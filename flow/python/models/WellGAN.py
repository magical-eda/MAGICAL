import tensorflow.compat.v1 as tf
tf.disable_v2_behavior()
import numpy as np
from six.moves import xrange
import time
import os
from PIL import Image
import cv2
import scipy.misc
from glob import glob
import imageio
import random
import torch

cuda = False
#cuda = True if torch.cuda.is_available() else False
Tensor = torch.cuda.FloatTensor if cuda else torch.FloatTensor

def merge(images, size):
    h, w = images.shape[1], images.shape[2]
    img = np.zeros((h * size[0], w * size[1], 3))
    for idx, image in enumerate(images):
        i = idx % size[1]
        j = idx // size[1]
        img[j*h:j*h+h, i*w:i*w+w, :] = image

    return img


def load_data(image_path, flip=True, is_test=False):
    img_A, img_B = load_image(image_path)
    img_A, img_B = preprocess_A_and_B(img_A, img_B, flip=flip, is_test=is_test)

    img_A = img_A/127.5 - 1.
    img_B = img_B/127.5 - 1.
    return img_A,img_B

def load_data_concat(image_path, flip=True, is_test=False):
    img_A,img_B = load_data(image_path, flip, is_test)

    img_AB = np.concatenate((img_A, img_B), axis=2)
    # img_AB shape: (fine_size, fine_size, input_c_dim + output_c_dim)
    return img_AB

def load_image(image_path):
    input_img = cv2.imread(image_path)
    w = int(input_img.shape[1])
    w2 = int(w/2)
    img_A = input_img[:, 0:w2]
    img_B = input_img[:, w2:w]

    return img_A, img_B

def inverse_transform(images):
    return ((images+1.)/2.) * 255


def imsave(images, size, path):
    return imageio.imwrite(path, merge(images, size).astype(np.uint8))
def save_images(images, size, image_path):
    return imsave(inverse_transform(images), size, image_path)


def preprocess_A_and_B(img_A, img_B, load_size=286, fine_size=256, flip=True, is_test=False):
    if is_test:
        img_A = np.array(Image.fromarray(img_A).resize([fine_size, fine_size]))
        img_B = np.array(Image.fromarray(img_B).resize([fine_size, fine_size]))
    else:
        img_A = np.array(Image.fromarray(img_A).resize([load_size, load_size]))
        img_B = np.array(Image.fromarray(img_B).resize([load_size, load_size]))

        h1 = int(np.ceil(np.random.uniform(1e-2, load_size-fine_size)))
        w1 = int(np.ceil(np.random.uniform(1e-2, load_size-fine_size)))
        img_A = img_A[h1:h1+fine_size, w1:w1+fine_size]
        img_B = img_B[h1:h1+fine_size, w1:w1+fine_size]

        if flip and np.random.random() > 0.5:
            img_A = np.fliplr(img_A)
            img_B = np.fliplr(img_B)

    return img_A, img_B


class batch_norm(object):
    def __init__(self, epsilon=1e-5, momentum = 0.9, name="batch_norm"):
        with tf.variable_scope(name):
            self.epsilon = epsilon
            self.momentum = momentum
            self.name = name

    def __call__(self, x, train=True):
        return tf.layers.batch_normalization(x, momentum=self.momentum, epsilon=self.epsilon,  scale=True, name=self.name)


def binary_cross_entropy(preds, targets, name=None):
    """Computes binary cross entropy given `preds`.

    For brevity, let `x = `, `z = targets`.  The logistic loss is

        loss(x, z) = - sum_i (x[i] * log(z[i]) + (1 - x[i]) * log(1 - z[i]))

    Args:
        preds: A `Tensor` of type `float32` or `float64`.
        targets: A `Tensor` of the same type and shape as `preds`.
    """
    eps = 1e-12
    with ops.op_scope([preds, targets], name, "bce_loss") as name:
        preds = ops.convert_to_tensor(preds, name="preds")
        targets = ops.convert_to_tensor(targets, name="targets")
        return tf.reduce_mean(-(targets * tf.log(preds + eps) +
                              (1. - targets) * tf.log(1. - preds + eps)))

def conv_cond_concat(x, y):
    """Concatenate conditioning vector on feature map axis."""
    x_shapes = x.get_shape()
    y_shapes = y.get_shape()
    return tf.concat([x, y*tf.ones([x_shapes[0], x_shapes[1], x_shapes[2], y_shapes[3]])], 3)

def conv2d(input_, output_dim, 
           k_h=5, k_w=5, d_h=2, d_w=2, stddev=0.02,
           name="conv2d"):
    with tf.variable_scope(name):
        w = tf.get_variable('w', [k_h, k_w, input_.get_shape()[-1], output_dim],
                            initializer=tf.truncated_normal_initializer(stddev=stddev))
        conv = tf.nn.conv2d(input_, w, strides=[1, d_h, d_w, 1], padding='SAME')

        biases = tf.get_variable('biases', [output_dim], initializer=tf.constant_initializer(0.0))
        conv = tf.reshape(tf.nn.bias_add(conv, biases), conv.get_shape())

        return conv

def deconv2d(input_, output_shape,
             k_h=5, k_w=5, d_h=2, d_w=2, stddev=0.02,
             name="deconv2d", with_w=False):
    with tf.variable_scope(name):
        # filter : [height, width, output_channels, in_channels]
        w = tf.get_variable('w', [k_h, k_w, output_shape[-1], input_.get_shape()[-1]],
                            initializer=tf.random_normal_initializer(stddev=stddev))
        
        try:
            deconv = tf.nn.conv2d_transpose(input_, w, output_shape=output_shape,
                                strides=[1, d_h, d_w, 1])

        # Support for verisons of TensorFlow before 0.7.0
        except AttributeError:
            deconv = tf.nn.deconv2d(input_, w, output_shape=output_shape,
                                strides=[1, d_h, d_w, 1])

        biases = tf.get_variable('biases', [output_shape[-1]], initializer=tf.constant_initializer(0.0))
        deconv = tf.reshape(tf.nn.bias_add(deconv, biases), deconv.get_shape())

        if with_w:
            return deconv, w, biases
        else:
            return deconv
       

def lrelu(x, leak=0.2, name="lrelu"):
  return tf.maximum(x, leak*x)

def linear(input_, output_size, scope=None, stddev=0.02, bias_start=0.0, with_w=False):
    shape = input_.get_shape().as_list()

    with tf.variable_scope(scope or "Linear"):
        matrix = tf.get_variable("Matrix", [shape[1], output_size], tf.float32,
                                 tf.random_normal_initializer(stddev=stddev))
        bias = tf.get_variable("bias", [output_size],
            initializer=tf.constant_initializer(bias_start))
        if with_w:
            return tf.matmul(input_, matrix) + bias, matrix, bias
        else:
            return tf.matmul(input_, matrix) + bias

class pix2pix(object):
    def __init__(self, sess, image_size=256,
                 batch_size=1, sample_size=1, output_size=256,
                 gf_dim=64, df_dim=64, L1_lambda=100,
                 input_c_dim=3, output_c_dim=3,
                 checkpoint_dir="checkpoint/wellGAN"):
        """

        Args:
            sess: TensorFlow session
            batch_size: The size of batch. Should be specified before training.
            output_size: (optional) The resolution in pixels of the images. [256]
            gf_dim: (optional) Dimension of gen filters in first conv layer. [64]
            df_dim: (optional) Dimension of discrim filters in first conv layer. [64]
            input_c_dim: (optional) Dimension of input image color. For grayscale input, set to 1. [3]
            output_c_dim: (optional) Dimension of output image color. For grayscale input, set to 1. [3]
        """
        self.sess = sess
        self.is_grayscale = (input_c_dim == 1)
        self.batch_size = batch_size
        self.image_size = image_size
        self.sample_size = sample_size
        self.output_size = output_size

        self.gf_dim = gf_dim
        self.df_dim = df_dim

        self.input_c_dim = input_c_dim
        self.output_c_dim = output_c_dim

        self.L1_lambda = L1_lambda

        # batch normalization : deals with poor initialization helps gradient flow
        self.d_bn1 = batch_norm(name='d_bn1')
        self.d_bn2 = batch_norm(name='d_bn2')
        self.d_bn3 = batch_norm(name='d_bn3')

        self.g_bn_e2 = batch_norm(name='g_bn_e2')
        self.g_bn_e3 = batch_norm(name='g_bn_e3')
        self.g_bn_e4 = batch_norm(name='g_bn_e4')
        self.g_bn_e5 = batch_norm(name='g_bn_e5')
        self.g_bn_e6 = batch_norm(name='g_bn_e6')
        self.g_bn_e7 = batch_norm(name='g_bn_e7')
        self.g_bn_e8 = batch_norm(name='g_bn_e8')

        self.g_bn_d1 = batch_norm(name='g_bn_d1')
        self.g_bn_d2 = batch_norm(name='g_bn_d2')
        self.g_bn_d3 = batch_norm(name='g_bn_d3')
        self.g_bn_d4 = batch_norm(name='g_bn_d4')
        self.g_bn_d5 = batch_norm(name='g_bn_d5')
        self.g_bn_d6 = batch_norm(name='g_bn_d6')
        self.g_bn_d7 = batch_norm(name='g_bn_d7')

        self.checkpoint_dir = os.path.dirname(os.path.abspath(__file__)) + '/' + checkpoint_dir
        self.sample_dir = '/samples'
        self.train_size = 1e8
        self.epoch = 100000
        self.lr = 0.0002
        self.beta1 = 0.5
        self.dataset_name = "WellGen2_3"

        self.build_model()

    def build_model(self):
        self.real_data = tf.placeholder(tf.float32,
                                        [self.batch_size, self.image_size, self.image_size,
                                         self.input_c_dim + self.output_c_dim],
                                        name='real_A_and_B_images')

        self.real_B = self.real_data[:, :, :, :self.input_c_dim]
        self.real_A = self.real_data[:, :, :, self.input_c_dim:self.input_c_dim + self.output_c_dim]

        self.fake_B = self.generator(self.real_A)

        self.real_AB = tf.concat([self.real_A, self.real_B], 3)
        self.fake_AB = tf.concat([self.real_A, self.fake_B], 3)
        self.D, self.D_logits = self.discriminator(self.real_AB, reuse=False)
        self.D_, self.D_logits_ = self.discriminator(self.fake_AB, reuse=True)

        self.fake_B_sample = self.sampler(self.real_A)

        self.d_sum = tf.summary.histogram("d", self.D)
        self.d__sum = tf.summary.histogram("d_", self.D_)
        self.fake_B_sum = tf.summary.image("fake_B", self.fake_B)

        self.d_loss_real = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(logits=self.D_logits, labels=tf.ones_like(self.D)))
        self.d_loss_fake = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(logits=self.D_logits_, labels=tf.zeros_like(self.D_)))
        self.g_loss = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(logits=self.D_logits_, labels=tf.ones_like(self.D_))) \
                        + self.L1_lambda * tf.reduce_mean(tf.abs(self.real_B - self.fake_B))

        self.d_loss_real_sum = tf.summary.scalar("d_loss_real", self.d_loss_real)
        self.d_loss_fake_sum = tf.summary.scalar("d_loss_fake", self.d_loss_fake)

        self.d_loss = self.d_loss_real + self.d_loss_fake

        self.g_loss_sum = tf.summary.scalar("g_loss", self.g_loss)
        self.d_loss_sum = tf.summary.scalar("d_loss", self.d_loss)

        t_vars = tf.trainable_variables()

        self.d_vars = [var for var in t_vars if 'd_' in var.name]
        self.g_vars = [var for var in t_vars if 'g_' in var.name]



    def discriminator(self, image, y=None, reuse=False):

        with tf.variable_scope("discriminator") as scope:

            # image is 256 x 256 x (input_c_dim + output_c_dim)
            if reuse:
                tf.get_variable_scope().reuse_variables()
            else:
                assert tf.get_variable_scope().reuse == False

            h0 = lrelu(conv2d(image, self.df_dim, name='d_h0_conv'))
            # h0 is (128 x 128 x self.df_dim)
            h1 = lrelu(self.d_bn1(conv2d(h0, self.df_dim*2, name='d_h1_conv')))
            # h1 is (64 x 64 x self.df_dim*2)
            h2 = lrelu(self.d_bn2(conv2d(h1, self.df_dim*4, name='d_h2_conv')))
            # h2 is (32x 32 x self.df_dim*4)
            h3 = lrelu(self.d_bn3(conv2d(h2, self.df_dim*8, d_h=1, d_w=1, name='d_h3_conv')))
            # h3 is (16 x 16 x self.df_dim*8)
            h4 = linear(tf.reshape(h3, [self.batch_size, -1]), 1, 'd_h3_lin')

            return tf.nn.sigmoid(h4), h4

    def generator(self, image, y=None):
        with tf.variable_scope("generator") as scope:

            s = self.output_size
            s2, s4, s8, s16, s32, s64, s128 = int(s/2), int(s/4), int(s/8), int(s/16), int(s/32), int(s/64), int(s/128)

            # image is (256 x 256 x input_c_dim)
            e1 = conv2d(image, self.gf_dim, name='g_e1_conv')
            # e1 is (128 x 128 x self.gf_dim)
            e2 = self.g_bn_e2(conv2d(lrelu(e1), self.gf_dim*2, name='g_e2_conv'))
            # e2 is (64 x 64 x self.gf_dim*2)
            e3 = self.g_bn_e3(conv2d(lrelu(e2), self.gf_dim*4, name='g_e3_conv'))
            # e3 is (32 x 32 x self.gf_dim*4)
            e4 = self.g_bn_e4(conv2d(lrelu(e3), self.gf_dim*8, name='g_e4_conv'))
            # e4 is (16 x 16 x self.gf_dim*8)
            e5 = self.g_bn_e5(conv2d(lrelu(e4), self.gf_dim*8, name='g_e5_conv'))
            # e5 is (8 x 8 x self.gf_dim*8)
            e6 = self.g_bn_e6(conv2d(lrelu(e5), self.gf_dim*8, name='g_e6_conv'))
            # e6 is (4 x 4 x self.gf_dim*8)
            e7 = self.g_bn_e7(conv2d(lrelu(e6), self.gf_dim*8, name='g_e7_conv'))
            # e7 is (2 x 2 x self.gf_dim*8)
            e8 = self.g_bn_e8(conv2d(lrelu(e7), self.gf_dim*8, name='g_e8_conv'))
            # e8 is (1 x 1 x self.gf_dim*8)

            self.d1, self.d1_w, self.d1_b = deconv2d(tf.nn.relu(e8),
                [self.batch_size, s128, s128, self.gf_dim*8], name='g_d1', with_w=True)
            d1 = tf.nn.dropout(self.g_bn_d1(self.d1), 0.5)
            d1 = tf.concat([d1, e7], 3)
            # d1 is (2 x 2 x self.gf_dim*8*2)

            self.d2, self.d2_w, self.d2_b = deconv2d(tf.nn.relu(d1),
                [self.batch_size, s64, s64, self.gf_dim*8], name='g_d2', with_w=True)
            d2 = tf.nn.dropout(self.g_bn_d2(self.d2), 0.5)
            d2 = tf.concat([d2, e6], 3)
            # d2 is (4 x 4 x self.gf_dim*8*2)

            self.d3, self.d3_w, self.d3_b = deconv2d(tf.nn.relu(d2),
                [self.batch_size, s32, s32, self.gf_dim*8], name='g_d3', with_w=True)
            d3 = tf.nn.dropout(self.g_bn_d3(self.d3), 0.5)
            d3 = tf.concat([d3, e5], 3)
            # d3 is (8 x 8 x self.gf_dim*8*2)

            self.d4, self.d4_w, self.d4_b = deconv2d(tf.nn.relu(d3),
                [self.batch_size, s16, s16, self.gf_dim*8], name='g_d4', with_w=True)
            d4 = self.g_bn_d4(self.d4)
            d4 = tf.concat([d4, e4], 3)
            # d4 is (16 x 16 x self.gf_dim*8*2)

            self.d5, self.d5_w, self.d5_b = deconv2d(tf.nn.relu(d4),
                [self.batch_size, s8, s8, self.gf_dim*4], name='g_d5', with_w=True)
            d5 = self.g_bn_d5(self.d5)
            d5 = tf.concat([d5, e3], 3)
            # d5 is (32 x 32 x self.gf_dim*4*2)

            self.d6, self.d6_w, self.d6_b = deconv2d(tf.nn.relu(d5),
                [self.batch_size, s4, s4, self.gf_dim*2], name='g_d6', with_w=True)
            d6 = self.g_bn_d6(self.d6)
            d6 = tf.concat([d6, e2], 3)
            # d6 is (64 x 64 x self.gf_dim*2*2)

            self.d7, self.d7_w, self.d7_b = deconv2d(tf.nn.relu(d6),
                [self.batch_size, s2, s2, self.gf_dim], name='g_d7', with_w=True)
            d7 = self.g_bn_d7(self.d7)
            d7 = tf.concat([d7, e1], 3)
            # d7 is (128 x 128 x self.gf_dim*1*2)

            self.d8, self.d8_w, self.d8_b = deconv2d(tf.nn.relu(d7),
                [self.batch_size, s, s, self.output_c_dim], name='g_d8', with_w=True)
            # d8 is (256 x 256 x output_c_dim)

            return tf.nn.tanh(self.d8)

    def sampler(self, image, y=None):

        with tf.variable_scope("generator") as scope:
            scope.reuse_variables()

            s = self.output_size
            s2, s4, s8, s16, s32, s64, s128 = int(s/2), int(s/4), int(s/8), int(s/16), int(s/32), int(s/64), int(s/128)


            # image is (256 x 256 x input_c_dim)
            e1 = conv2d(image, self.gf_dim, name='g_e1_conv')
            # e1 is (128 x 128 x self.gf_dim)
            e2 = self.g_bn_e2(conv2d(lrelu(e1), self.gf_dim*2, name='g_e2_conv'))
            # e2 is (64 x 64 x self.gf_dim*2)
            e3 = self.g_bn_e3(conv2d(lrelu(e2), self.gf_dim*4, name='g_e3_conv'))
            # e3 is (32 x 32 x self.gf_dim*4)
            e4 = self.g_bn_e4(conv2d(lrelu(e3), self.gf_dim*8, name='g_e4_conv'))
            # e4 is (16 x 16 x self.gf_dim*8)
            e5 = self.g_bn_e5(conv2d(lrelu(e4), self.gf_dim*8, name='g_e5_conv'))
            # e5 is (8 x 8 x self.gf_dim*8)
            e6 = self.g_bn_e6(conv2d(lrelu(e5), self.gf_dim*8, name='g_e6_conv'))
            # e6 is (4 x 4 x self.gf_dim*8)
            e7 = self.g_bn_e7(conv2d(lrelu(e6), self.gf_dim*8, name='g_e7_conv'))
            # e7 is (2 x 2 x self.gf_dim*8)
            e8 = self.g_bn_e8(conv2d(lrelu(e7), self.gf_dim*8, name='g_e8_conv'))
            #e8  = conv2d(lrelu(e7), self.gf_dim*8, name='g_e8_conv')
            # e8 is (1 x 1 x self.gf_dim*8)

            self.ccc = e8

            self.d1, self.d1_w, self.d1_b = deconv2d(tf.nn.relu(e8),
                [self.batch_size, s128, s128, self.gf_dim*8], name='g_d1', with_w=True)
            d1 = tf.nn.dropout(self.g_bn_d1(self.d1), 0.5)
            d1 = tf.concat([d1, e7], 3)
            # d1 is (2 x 2 x self.gf_dim*8*2)

            self.d2, self.d2_w, self.d2_b = deconv2d(tf.nn.relu(d1),
                [self.batch_size, s64, s64, self.gf_dim*8], name='g_d2', with_w=True)
            d2 = tf.nn.dropout(self.g_bn_d2(self.d2), 0.5)
            d2 = tf.concat([d2, e6], 3)
            # d2 is (4 x 4 x self.gf_dim*8*2)

            self.d3, self.d3_w, self.d3_b = deconv2d(tf.nn.relu(d2),
                [self.batch_size, s32, s32, self.gf_dim*8], name='g_d3', with_w=True)
            d3 = tf.nn.dropout(self.g_bn_d3(self.d3), 0.5)
            d3 = tf.concat([d3, e5], 3)
            # d3 is (8 x 8 x self.gf_dim*8*2)

            self.d4, self.d4_w, self.d4_b = deconv2d(tf.nn.relu(d3),
                [self.batch_size, s16, s16, self.gf_dim*8], name='g_d4', with_w=True)
            d4 = self.g_bn_d4(self.d4)
            d4 = tf.concat([d4, e4], 3)
            # d4 is (16 x 16 x self.gf_dim*8*2)

            self.d5, self.d5_w, self.d5_b = deconv2d(tf.nn.relu(d4),
                [self.batch_size, s8, s8, self.gf_dim*4], name='g_d5', with_w=True)
            d5 = self.g_bn_d5(self.d5)
            d5 = tf.concat([d5, e3], 3)
            # d5 is (32 x 32 x self.gf_dim*4*2)

            self.d6, self.d6_w, self.d6_b = deconv2d(tf.nn.relu(d5),
                [self.batch_size, s4, s4, self.gf_dim*2], name='g_d6', with_w=True)
            d6 = self.g_bn_d6(self.d6)
            d6 = tf.concat([d6, e2], 3)
            # d6 is (64 x 64 x self.gf_dim*2*2)

            self.d7, self.d7_w, self.d7_b = deconv2d(tf.nn.relu(d6),
                [self.batch_size, s2, s2, self.gf_dim], name='g_d7', with_w=True)
            d7 = self.g_bn_d7(self.d7)
            d7 = tf.concat([d7, e1], 3)
            # d7 is (128 x 128 x self.gf_dim*1*2)

            self.d8, self.d8_w, self.d8_b = deconv2d(tf.nn.relu(d7),
                [self.batch_size, s, s, self.output_c_dim], name='g_d8', with_w=True)
            # d8 is (256 x 256 x output_c_dim)


            return tf.nn.tanh(self.d8)


    def load(self):
        return
        saver = tf.train.Saver()
        saver.restore(self.sess, self.checkpoint_dir)
    def save(self):
        saver = tf.train.Saver()
        saver.save(self.sess, self.checkpoint_dir)

    def test(self, imgs):
        """Test pix2pix"""
        init_op = tf.global_variables_initializer()
        self.sess.run(init_op)

        sample_images = imgs

        sample_images = [sample_images[i:i+self.batch_size]
                         for i in xrange(0, len(sample_images), self.batch_size)]
        sample_images = np.array(sample_images)

        start_time = time.time()
        self.load()

        samples = []

        print(sample_images.shape)

        for i, sample_image in enumerate(sample_images):
            idx = i+1
            sample, ccc = self.sess.run(
                [self.fake_B_sample, self.ccc],
                feed_dict={self.real_data: sample_image}
            )
            print(ccc)
            samples.append(sample[0])
        self.samples = np.array(samples)
        return self.samples

    def load_random_samples(self):
        # data = np.random.choice(glob('./datasets/{}/val/*.jpg'.format(self.dataset_name)), self.batch_size)
        data = np.random.choice(glob('./datasets/{}/val/*.png'.format(self.dataset_name)), self.batch_size)
        sample = [load_data_concat(sample_file) for sample_file in data]

        if (self.is_grayscale):
            sample_images = np.array(sample).astype(np.float32)[:, :, :, None]
        else:
            sample_images = np.array(sample).astype(np.float32)
        return sample_images

    def sample_model(self, sample_dir, epoch, idx):
        sample_images = self.load_random_samples()
        samples, d_loss, g_loss = self.sess.run(
            [self.fake_B_sample, self.d_loss, self.g_loss],
            feed_dict={self.real_data: sample_images}
        )
        save_images(samples, [self.batch_size, 1],
                    './{}/train_{:02d}_{:04d}.png'.format(sample_dir, epoch, idx))
        print("[Sample] d_loss: {:.8f}, g_loss: {:.8f}".format(d_loss, g_loss))

    def train(self):
        """Train pix2pix"""
        d_optim = tf.train.AdamOptimizer(self.lr, beta1=self.beta1) \
                          .minimize(self.d_loss, var_list=self.d_vars)
        g_optim = tf.train.AdamOptimizer(self.lr, beta1=self.beta1) \
                          .minimize(self.g_loss, var_list=self.g_vars)

        init_op = tf.global_variables_initializer()
        self.sess.run(init_op)

        self.g_sum = tf.summary.merge([self.d__sum,
            self.fake_B_sum, self.d_loss_fake_sum, self.g_loss_sum])
        self.d_sum = tf.summary.merge([self.d_sum, self.d_loss_real_sum, self.d_loss_sum])
        self.writer = tf.summary.FileWriter("./logs", self.sess.graph)

        counter = 1
        start_time = time.time()

        if self.load():
            print(" [*] Load SUCCESS")
        else:
            print(" [!] Load failed...")

        for epoch in xrange(self.epoch):
            # data = glob('./datasets/{}/train/*.jpg'.format(self.dataset_name))
            data = glob('./datasets/{}/train/*.png'.format(self.dataset_name))
            #np.random.shuffle(data)
            batch_idxs = min(len(data), self.train_size) // self.batch_size
            batch_idxs = 1

            for idx in xrange(0, batch_idxs):
                batch_files = data[idx*self.batch_size:(idx+1)*self.batch_size]
                batch = [load_data_concat(batch_file) for batch_file in batch_files]
                if (self.is_grayscale):
                    batch_images = np.array(batch).astype(np.float32)[:, :, :, None]
                else:
                    batch_images = np.array(batch).astype(np.float32)

                # Update D network
                _, summary_str = self.sess.run([d_optim, self.d_sum],
                                               feed_dict={ self.real_data: batch_images })
                self.writer.add_summary(summary_str, counter)

                # Update G network
                _, summary_str = self.sess.run([g_optim, self.g_sum],
                                               feed_dict={ self.real_data: batch_images })
                self.writer.add_summary(summary_str, counter)

                # Run g_optim twice to make sure that d_loss does not go to zero (different from paper)
                _, summary_str = self.sess.run([g_optim, self.g_sum],
                                               feed_dict={ self.real_data: batch_images })
                self.writer.add_summary(summary_str, counter)

                errD_fake = self.d_loss_fake.eval({self.real_data: batch_images})
                errD_real = self.d_loss_real.eval({self.real_data: batch_images})
                errG = self.g_loss.eval({self.real_data: batch_images})

                counter += 1
                print("Epoch: [%2d] [%4d/%4d] time: %4.4f, d_loss: %.8f, g_loss: %.8f" \
                    % (epoch, idx, batch_idxs,
                        time.time() - start_time, errD_fake+errD_real, errG))

                if np.mod(counter, 100) == 1:
                    self.sample_model(self.sample_dir, epoch, idx)

                if np.mod(counter, 500) == 2:
                    self.save()

def swap_to_model(arr):
    # [N, W, H, C] -> [N, C, W, H]
    arr = np.swapaxes(arr, 2, 3)
    arr = np.swapaxes(arr, 1, 2) 
    return torch.from_numpy(arr)

def swap_to_img(tensor):
    # [N, C, W, H] -> [N, W, H, c]
    arr = tensor.cpu().detach().numpy()
    arr = np.swapaxes(arr, 1, 2)
    arr = np.swapaxes(arr, 2, 3) 
    return arr

class Generator(torch.nn.Module):
    def __init__(self, gf_dim):
        super(Generator, self).__init__()
        self.encoder = torch.nn.Sequential(
                torch.nn.BatchNorm2d(3),
                torch.nn.Conv2d(3, gf_dim, 3, stride=2, padding=1),
                torch.nn.BatchNorm2d(gf_dim, 0.8),
                torch.nn.LeakyReLU(0.2, inplace=True),
                torch.nn.Conv2d(gf_dim, gf_dim*2, 3, stride=2, padding=1),
                torch.nn.BatchNorm2d(gf_dim*2, 0.8),
                torch.nn.LeakyReLU(0.2, inplace=True),
                torch.nn.Conv2d(gf_dim*2, gf_dim*4, 3, stride=2, padding=1),
                torch.nn.BatchNorm2d(gf_dim*4, 0.8),
                torch.nn.LeakyReLU(0.2, inplace=True),
                torch.nn.MaxPool2d(2, stride=1, padding=1)
                )
        self.decoder = torch.nn.Sequential(
                torch.nn.ConvTranspose2d(gf_dim*4, gf_dim*2, 3, stride=2, padding=1),
                torch.nn.LeakyReLU(0.2, inplace=True),
                torch.nn.ConvTranspose2d(gf_dim*2, gf_dim, 3, stride=2, padding=1),
                torch.nn.LeakyReLU(0.2, inplace=True),
                torch.nn.ConvTranspose2d(gf_dim, 3, 2, stride=2, padding=1),
                torch.nn.LeakyReLU(0.2, inplace=True),
                torch.nn.Tanh()
                )

    def forward(self, image):
        x = self.encoder(image)
        x = self.decoder(x)
        return x

class Discriminator(torch.nn.Module):
    def __init__(self):
        super(Discriminator, self).__init__()

        def _block(in_filters, out_filters, bn=True):
            block = [torch.nn.Conv2d(in_filters, out_filters, 3, 2, 1), torch.nn.LeakyReLU(0.2, inplace=True), torch.nn.Dropout2d(0.25)]
            if bn:
                block.append(torch.nn.BatchNorm2d(out_filters, 0.8))
            return block

        self.model = torch.nn.Sequential(
                *_block(6, 16, bn=False),
                *_block(16, 32),
                *_block(32, 64),
                *_block(64, 128)
                )

        ds_size = 256 // 2 ** 4
        self.adv_layer = torch.nn.Sequential(torch.nn.Linear(128 * ds_size ** 2, 1), torch.nn.Sigmoid())

    def forward(self, img):
        out = self.model(img)
        out = out.view(out.shape[0], -1)
        validity = self.adv_layer(out)
        return validity

class torch_p2p(object):
    def __init__(self):
        self.batch_size = 10
        self.image_size = 256
        self.sample_size = 1
        self.output_size = 256

        self.gf_dim = 64

        self.input_c_dim = 3
        self.output_c_dim = 3

        self.L1_lambda = 100

        self.checkpoint_dir = os.path.dirname(os.path.abspath(__file__)) + '/'  + "checkpoint/wellGAN"
        self.sample_dir = '/samples'
        self.train_size = 1e8
        self.epoch = 100000
        self.lr = 0.0002
        self.beta1 = 0.5
        self.beta2 = 0.8
        self.dataset_name = "WellGen2_3"

        self.G = Generator(self.gf_dim)
        self.D = Discriminator()


        self.optm_g = torch.optim.Adam(self.G.parameters(), lr = self.lr, betas=(self.beta1, self.beta2))
        self.optm_d = torch.optim.Adam(self.D.parameters(), lr = self.lr, betas=(self.beta1, self.beta2))

        self.adv_loss = torch.nn.BCELoss()
        self.mse_loss = torch.nn.MSELoss()

        if cuda:
            self.G.cuda()
            self.D.cuda()
            self.adv_loss.cuda()
            self.mse_loss.cuda()

    def optimize(self, imgs):
        """
        Channel should be 2
        """
        self.D.train()
        self.G.train()
        valid = torch.autograd.Variable(Tensor(imgs.shape[0], 1).fill_(1.0), requires_grad=False)
        fake = torch.autograd.Variable(Tensor(imgs.shape[0], 1).fill_(0.0), requires_grad=False)
        real_B = imgs[:, :, :, :self.input_c_dim]
        real_A = imgs[:, :, :, self.input_c_dim:self.input_c_dim + self.output_c_dim]
        real_B = swap_to_model(real_B)
        real_A = swap_to_model(real_A)

        fake_B = self.G(real_A)
        
        real_AB = torch.cat((real_A, real_B), dim = 1)
        fake_AB = torch.cat((real_A, fake_B), dim = 1)

        self.optm_g.zero_grad()
        self.g_loss = self.adv_loss(self.D(fake_AB), valid) + self.L1_lambda * self.mse_loss(real_B, fake_B)

        self.g_loss.backward()
        self.optm_g.step()

        self.optm_d.zero_grad()
        real_loss = self.adv_loss(self.D(real_AB), valid)
        fake_loss = self.adv_loss(self.D(fake_AB.detach()), fake)
        self.d_loss = real_loss + fake_loss

        self.d_loss.backward()
        self.optm_d.step()


    def sample(self, img):
        """
        Channel should be 3
        """
        self.D.eval()
        self.G.eval()
        img = img[:, :, :, self.input_c_dim:self.input_c_dim + self.output_c_dim]
        img = swap_to_model(img)
        tensor = self.G(img)
        arr = swap_to_img(tensor)
        return arr

    def save_model(self):
        torch.save({
            'discriminator_state_dict' : self.D.state_dict(),
            'generator_state_dict' : self.G.state_dict(),
            'optmD_state_dict' : self.optm_d.state_dict(),
            'optmG_state_dict' : self.optm_g.state_dict(),
                }, self.checkpoint_dir)

    def load_model(self):
        checkpoint = torch.load(self.checkpoint_dir)
        self.D.load_state_dict(checkpoint['discriminator_state_dict'])
        self.G.load_state_dict(checkpoint['generator_state_dict'])
        self.optm_d.load_state_dict(checkpoint['optmD_state_dict'])
        self.optm_g.load_state_dict(checkpoint['optmG_state_dict'])


    


def validate_from_dataset(img):
    """
    Try a WellGAN dataset image
    """
    imgA, imgB = load_data(img, is_test=True)
    sample =  []
    sample.append(np.concatenate((imgA, imgB), axis=2))
    with tf.Session() as sess:
        model = pix2pix(sess)
        infer = model.test(sample) 
        #infer[:,:,:,2] = -1
        #infer[:,:,:,1] = -1
        img = infer[0]
        img = img /2.0 + 0.5
        img = (img[:,:, :3] * 255).astype(np.uint8)
        b,g,r = img[:,:,0], img[:,:,1], img[:,:,2]
        img = np.concatenate((r[:,:,np.newaxis],g[:,:, np.newaxis],b[:,:, np.newaxis]), axis=-1)
        img_s = Image.fromarray(img, 'RGB') # fromarray only works with uint8
        img_s.show()


def train():
    batch_size = 10
    model = torch_p2p()
    #model.load_model()
    dataset_name = "WellGen2_3"
    epoch = 1000
    sample_dir = "samples"

    counter = 1
    start_time = time.time()

    for epoch in xrange(epoch):
        # data = glob('./datasets/{}/train/*.jpg'.format(self.dataset_name))
        data = glob('./datasets/{}/train/*.png'.format(dataset_name))
        np.random.shuffle(data)
        batch_idxs = min(len(data), 1e8) // batch_size

        for idx in xrange(0, batch_idxs):
            batch_files = data[idx*batch_size:(idx+1)*batch_size]
            batch = [load_data_concat(batch_file) for batch_file in batch_files]
            batch_images = np.array(batch).astype(np.float32)

            model.optimize(batch_images)

            d_loss = model.d_loss
            g_loss = model.g_loss

            counter += 1
            print("Epoch: [%2d] [%4d/%4d] time: %4.4f, d_loss: %.8f, g_loss: %.8f" \
                % (epoch, idx, batch_idxs,
                    time.time() - start_time, d_loss, g_loss))

            if np.mod(counter, 100) == 1:
                validata = np.random.choice(glob('./datasets/{}/val/*.png'.format(dataset_name)), 1)
                sample = [load_data_concat(val) for val in validata]
                sample_images = np.array(sample).astype(np.float32)
                result = model.sample(sample_images)
                save_images(result, [batch_size, 1],
                            './{}/train_{:02d}_{:04d}.png'.format(sample_dir, epoch, idx))

            if np.mod(counter, 500) == 2:
                model.save_model()



if __name__ == "__main__":
    #validate_from_dataset("/home/local/eda09/keren/projects/well_generation/pix2pix-tensorflow/datasets/WellGen2_3/val/My_Core_0_0.png")
    train()
