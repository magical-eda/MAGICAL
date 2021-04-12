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

#cuda = False
cuda = True if torch.cuda.is_available() else False
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


def swap_to_model(arr):
    # [N, W, H, C] -> [N, C, W, H]
    arr = np.swapaxes(arr, 2, 3)
    arr = np.swapaxes(arr, 1, 2) 
    if cuda:
        return torch.from_numpy(arr).cuda()
    else:
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
        start = time.time()
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
