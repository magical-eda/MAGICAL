import numpy as np
import cv2
import sys, getopt
from matplotlib import pyplot as plt
from shapely.ops import cascaded_union, polygonize
import shapely.geometry as geometry
from scipy.spatial import Delaunay
import math
import pylab as pl

# 0: horizontal; 1: vertical
def classify_edge(p1, p2):
    delta_x = abs(p1[0] - p2[0])
    delta_y = abs(p1[1] - p2[1])
    return delta_x < delta_y

def classify_edges(polygon):
    num_points = polygon.shape[0]
    # print "num_points: ", num_points
    # print "first point: ", polygon[0][0]
    # print "last point: ", polygon[-1][0]

    # coords = np.array([point[0] for point in points])
    edges = []
    edges_class = []
    for i in range(num_points-1):
        p1 = polygon[i][0]
        p2 = polygon[i+1][0]
        edges.append([p1, p2])
        edges_class.append( classify_edge(p1, p2) )
    edges.append([polygon[-1][0], polygon[0][0]])
    edges_class.append( classify_edge(polygon[-1][0], polygon[0][0]) )
    # print "num_edges: ", len(edges)
    return edges, edges_class

def merge_edges(edges, edges_class):
    # print "first edges: ", edges[0]
    # print "last edge: ", edges[-1]
    # print "edges: ", edges
    # print "edges_class: ", edges_class
    merged_edges = []
    merged_edges_class = []
    # merged_edges_value = []
    p1 = edges[0][0]
    p2 = edges[0][1]
    d = edges_class[0]
    if (p1 == p2).all(): # same point error
        # return merged_edges, merged_edges_class, merged_edges_value
        return merged_edges, merged_edges_class
    for i in range(1, len(edges) + 1):
        # print "edges[i]: ", edges[i], ", edges_class[i]: ", edges_class[i], ", d: ", d
        if i < len(edges_class) and edges_class[i] == d:
            # calculate area
            p2 = edges[i][1]
        else:
            # print "here1 p1: ", p1, ", p2: ", p2
            actual_d = classify_edge(p1, p2)
            merged_edges.append(i-1) # only append the index, the second point of this index is p2
            # merged_edges_class.append(d)
            merged_edges_class.append(actual_d)
            if i < len(edges_class):
                p1 = edges[i][0]
                p2 = edges[i][1]
                d = edges_class[i]
    
    # return merged_edges, merged_edges_class, merged_edges_value
    return merged_edges, merged_edges_class 

def calculate_edges_value(edges, i1, i2, d):
    tot_area = 0
    p1 = edges[i1][1]
    p2 = edges[i2][1]
    # print "i1: ", i1, ", i2: ", i2
    # print "p1: ", p1, ", p2: ", p2
    if i1 > i2:
        for i in range(i1+1, len(edges)):
            pointa = edges[i][0]
            pointb = edges[i][1]
            if d == 0:
                a = pointa[1] - p1[1]
                b = pointb[1] - p1[1]
                c = pointb[0] - pointa[0]
            else:
                a = pointa[0] - p1[0]
                b = pointb[0] - p1[0]
                c = pointb[1] - pointa[1]
            tot_area += (a + b) * c / 2.0        
        for i in range(0, i2+1):
            pointa = edges[i][0]
            pointb = edges[i][1]
            if d == 0:
                a = pointa[1] - p1[1]
                b = pointb[1] - p1[1]
                c = pointb[0] - pointa[0]
            else:
                a = pointa[0] - p1[0]
                b = pointb[0] - p1[0]
                c = pointb[1] - pointa[1]
            tot_area += (a + b) * c / 2.0        
    else:
        for i in range(i1+1, i2+1):
            pointa = edges[i][0]
            pointb = edges[i][1]
            if d == 0:
                a = pointa[1] - p1[1]
                b = pointb[1] - p1[1]
                c = pointb[0] - pointa[0]
            else:
                a = pointa[0] - p1[0]
                b = pointb[0] - p1[0]
                c = pointb[1] - pointa[1]
            tot_area += (a + b) * c / 2.0
    if d == 0:
        dist = p2[0] - p1[0]
        # print "tot_area: ", tot_area, ", dist: ", dist
        return p1[1] + int(tot_area / dist)
    else:
        dist = p2[1] - p1[1]
        # print "tot_area: ", tot_area, ", dist: ", dist
        return p1[0] + int(tot_area / dist)

def get_merged_edges_value(edges, merged_edges, merged_edges_class):
    # print "get_merged_edges_value..."
    ret_edges = []
    ret_value = []
    if len(merged_edges) == 1:
        ret_edges.append( edges[merged_edges[0]] )
        ret_value.append( calculate_edges_value(edges, merged_edges[0]-1, merged_edges[0], merged_edges_class[0]) )
        return ret_edges, ret_value
    for i in range(len(merged_edges)):
        i1 = merged_edges[i-1]
        i2 = merged_edges[i]
        p1 = edges[i1][1]
        p2 = edges[i2][1]
        ret_edges.append([p1, p2])
        ret_value.append( calculate_edges_value(edges, i1, i2, merged_edges_class[i]) )

    return ret_edges, ret_value

def rectianoynmousear_transform(edges, edges_class, edges_value):
    # find intersection points
    num = len(edges_class)
    points = np.zeros((num+1, 1, 2), np.int32)
    if num == 0:
        return points
    # first point
    if edges_class[0]:
        points[0][0][0] = edges_value[0]        # x 
        points[0][0][1] = edges_value[num-1]    # y
        # print "vertical edge at x = ", edges_value[0]
    else:
        points[0][0][0] = edges_value[num-1] 
        points[0][0][1] = edges_value[0]
        # print "horizontal edge at y = ", edges_value[0]
    for i in range(1, num):
        if edges_class[i]:
            points[i][0][0] = edges_value[i]
            points[i][0][1] = edges_value[i-1]
            # print "vertical edge at x = ", edges_value[i]
        else:
            points[i][0][0] = edges_value[i-1]
            points[i][0][1] = edges_value[i]
            # print "horizontal edge at y = ", edges_value[i]
    # last point
    points[num][0] = points[0][0]
    return points

# merge the edges that have the same direction
def merge_edges_direction(edges, merged_edges, merged_edges_class):
    ret_edges = []
    ret_edges_class = []
    i = 0
    while i < len(merged_edges):
        d = merged_edges_class[i] 
        j = i # i is first index, j is second index
        while (j < len(merged_edges)-1 and merged_edges_class[j+1] == d): 
            j += 1
        # if len(ret_edges) == 0:
        #     p1 = edges[0][0]
        # else:
        p1 = edges[merged_edges[i-1]][1]
        p2 = edges[merged_edges[j]][1]
        actual_d = classify_edge(p1, p2)
        ret_edges.append(merged_edges[j])
        ret_edges_class.append(actual_d)
        i = j+1
    # merge the first and the last edges if necessary
    if len(ret_edges_class) > 1 and ret_edges_class[0] == ret_edges_class[-1]:
        p1 = edges[ret_edges[-2]][1]
        p2 = edges[ret_edges[0]][1]
        actual_d = classify_edge(p1, p2)
        ret_edges_class[0] = actual_d
        ret_edges.pop()
        ret_edges_class.pop()
        
    return ret_edges, ret_edges_class

def check_alternate_direction(edges_class):
    # print "edges_class: ", edges_class
    for i in range(len(edges_class)):
        if edges_class[i-1] == edges_class[i]:
            return False
    return True

def orthogonalize(polygon):
    # classify edge to the two orthogonal direction
    edges, edges_class = classify_edges(polygon)
    # print "num_edges: ", len(edges)

    # merge adjacent edges that share the same class
    merged_edges, merged_edges_class = merge_edges(edges, edges_class)
    # print "num_merged_edges: ", len(merged_edges)

    while (len(merged_edges_class) > 1 and check_alternate_direction(merged_edges_class) == False):
        # print "not alternate direction"
        merged_edges, merged_edges_class = merge_edges_direction(edges, merged_edges, merged_edges_class)
    # print "num_merged_edges: ", len(merged_edges)
    
    if len(merged_edges) <= 1:
        return np.zeros((0, 1, 2), np.int32)

    merged_edges, merged_edges_value = get_merged_edges_value(edges, merged_edges, merged_edges_class)
    
    # FIXME
    num = len(merged_edges)
    points_merged = np.zeros((num+1, 1, 2), np.int32)
    for i in range(num):
        points_merged[i][0] = merged_edges[i][0]
    points_merged[num][0] = merged_edges[num-1][1] 
    # print "points_merged: ", points_merged
    
    # rectianoynmousear transformation: 
    # project on corresponding direction and find the intersection vertices
    points = rectianoynmousear_transform(merged_edges, merged_edges_class, merged_edges_value)
    # print "points: ", points

    return points

"""
def align_to_grid(contour, point, grid_size):
    orig_x = point[0]
    orig_y = point[1]
    aligned_contour = np.zeros(contour.shape, np.int32)
    for i in range(contour.shape[0]):
        pt_x = contour[i][0][0]
        pt_y = contour[i][0][1]
        grid_x = float(pt_x - orig_x) / float(grid_size)
        grid_y = float(pt_y - orig_y) / float(grid_size)
        grid_x = int(round(grid_x))
        grid_y = int(round(grid_y))
        new_x = orig_x + grid_x * grid_size
        new_y = orig_y + grid_y * grid_size
        aligned_contour[i][0][0] = new_x
        aligned_contour[i][0][1] = new_y
    return aligned_contour

# == Grid-based legalization == #
def legalize_grid(contour, grid_size):
    print contour
    # TODO: currently build grid according a random point (first point), need to change to select the most critical edges
    aligned_contour = align_to_grid(contour, contour[0][0], grid_size)
    return aligned_contour
"""

def legalize(infile, add, infile_d1):
    im = cv2.imread(infile)
    width, height, chan = im.shape

    offset_x = 10
    offset_y = 10
    im_padded = np.zeros((width+2*offset_x, height+2*offset_y, chan), np.uint8)
    im_padded[offset_x:width+offset_x, offset_y:height+offset_y] = im
    im = im_padded
    width, height, chan = im.shape

    # find the contours
    # thresh = cv2.adaptiveThreshold(im[:,:,0], 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY,11,2)
    ret, thresh = cv2.threshold(im[:, :, 0], 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    im2, contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    # contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
    imcon = np.zeros((width, height, chan), np.uint8)
    cv2.drawContours(imcon, contours, -1, (255,0,0), 3)
    approx_contours = []
    print len(contours)
    area_thresh = 20
    # cnt_debug = 0
    for contour in contours:
        if contour.shape[0] > 1 and cv2.contourArea(contour) > area_thresh:
            # print "contour area: ", cv2.contourArea(contour)
            approx = orthogonalize(contour)
            if approx.shape[0] <= 2:
                continue
            # epsilon = 0.015 * cv2.arcLength(points, True)
            epsilon = 3
            approx = cv2.approxPolyDP(approx, epsilon, True)
            # FIXME: no need to add the last point for orthogonalize() function
            # approx = np.concatenate((approx, approx[0:1, :, :]), axis=0)
            # print "approx contour area: ", cv2.contourArea(approx)
            approx = orthogonalize(approx)
            if approx.shape[0] > 2:
                approx_contours.append(approx)
                # cnt_debug += 1
                # if cnt_debug > 0:
                #     break
        # find_approx(approx_contours, contour)
    """
    grid_size = 8 # FIXME: minimum width rule
    aligned_approx_contours = []
    for contour in approx_contours:
        # legalize with grid and the features
        aligned_contour = legalize_grid(contour, grid_size)
        aligned_approx_contours.append(aligned_contour)
    """
    cname = infile.split('/')[-1][:-4]
    ctr_file = add+cname+".contours.approx"
    fctr = open(ctr_file, 'w')
    for contour in approx_contours:
        for point in contour:
            fctr.write(str(point[0][0] - offset_x)) 
            fctr.write(" ")
            fctr.write(str(point[0][1] - offset_y)) 
            fctr.write(" ")
        fctr.write("\n")
    fctr.close()

    imapprox = np.zeros((width, height, chan), np.uint8)
    # thickness is -1 will fill the shape
    cv2.drawContours(imapprox, approx_contours, -1, (255,0,0), -1)

    """
    imapprox_aligned = np.zeros((width, height, chan), np.uint8)
    # thickness is -1 will fill the shape
    cv2.drawContours(imapprox_aligned, aligned_approx_contours, -1, (255,0,0), -1)
    """
    
    imd1 = cv2.imread(infile_d1)
    # imd1_aligned = np.copy(imd1)
    # imd1[:, :, 0] = imapprox[:, :, 0] 
    imd1[:, :, 0] = imapprox[offset_x:width-offset_x, offset_y:height-offset_y, 0] 
    # imd1_aligned[:, :, 0] = imapprox_aligned[offset_x:width-offset_x, offset_y:height-offset_y, 0] 
    
    cv2.imwrite(add+cname+'_LEGALIZED.png', imd1)
    #cv2.imwrite(add+cname+'_ALIGNED.png', imd1_aligned)
    #cv2.imwrite(infile[:-4]+'_THRESH.png', thresh)
    #cv2.imwrite(infile[:-4]+'_CONTOUR.png', imcon)


def main(argv):
    try:
        opts, args = getopt.getopt(argv, "hf:a:d:", ["file_name=", "add_path", "d1_file_name"])
    except getopt.GetoptError:
        print 'legalize.py -f <file_name> -a <add_path> -d <d1_file_name>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'legalize.py -f <file_name> -a <add_path> -d <d1_file_name>'
            sys.exit()
        elif opt in ("-f", "--file_name"):
            fname = arg
        elif opt in ("-a", "--add_path"):
            add = arg
        elif opt in ("-d", "--d1_file_name"):
            fname_d1 = arg
    print fname_d1
    legalize(fname, add, fname_d1)


if __name__ == '__main__':
    main(sys.argv[1:])

