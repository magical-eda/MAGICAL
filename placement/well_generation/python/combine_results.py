import os
import sys, getopt
import numpy as np
import gdspy
import shutil
import pdb
import math
from PIL import Image, ImageDraw, ImageOps,ImageFilter

##save the coord in a list of lists

def read(infile,add='', golden=0, domain=2, desired_size=256):
    scale = 0.06 # 0.012  this number is used to scale the image to 256*256-> it depends on the original input size
    gdsii = gdspy.GdsLibrary()          # create a gpspy library
    gdsii.read_gds(add+'gdsii/'+infile) # read gds file and rename to avoid using the same name->. "TOP" is the original cell name
    print "length of top cells: ", len(gdsii.top_level())
    cell = gdsii.top_level()[0]
    print cell.name
    inPolygons = cell.get_polygons(True)            # read polygons-> inPolygons is a dictionary of arrays index by the layer number 
    bbox_x = cell.get_bounding_box()[:,0]   # get the bounding box of the design
    bbox_y = cell.get_bounding_box()[:,1]
    width  = int((bbox_x[1] - bbox_x[0])/scale)
    height = int((bbox_y[1] - bbox_y[0])/scale)
    img = Image.new('RGB', (width, height), 0)  # create an empty image, RGB 3 channels 
    # clip image
    print "width, height: ", width, ", ", height
    num_row = height/desired_size + 1
    num_col = width/desired_size + 1
    print "num_row, num_col: ", num_row, ", ", num_col
    for i in range(num_row):
        for j in range(num_col):
            if golden:
                fpath = add+'merged_images/'+cell.name+'_'+str(i)+'_'+str(j)+'.png'
            else:
                fpath = add+'test/'+cell.name+'_'+str(i)+'_'+str(j)+'.png'
            exists = os.path.isfile(fpath)
            if exists:
                # print "exists"
                sub_img = Image.open(fpath)
                crop_width = desired_size
                crop_height = desired_size
                if desired_size*(j+1) > width:
                    crop_width = width - desired_size*j
                if desired_size*(i+1) > height:
                    crop_height = height - desired_size*i
                # if crop_width != desired_size or crop_height != desired_size:
                if golden == 1 and domain == 1:
                    sub_img_crop = sub_img.crop((desired_size, 0, crop_width+desired_size, crop_height))
                else:
                    sub_img_crop = sub_img.crop((0, 0, crop_width, crop_height))
                img.paste(sub_img_crop, (desired_size*j, desired_size*i))
    if domain == 1:
        img.save(add+'combined_test_results_d1/'+cell.name+'.png')
    elif golden:
        img.save(add+'combined_test_results_golden/'+cell.name+'.png')
    else:
        img.save(add+'combined_test_results/'+cell.name+'.png')
		

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "hf:a:g:d:", ["file_name=", "add_path=", "test_golden", "image_domain"])
    except getopt.GetoptError:
        print 'combine_results.py -f <file_name> -a <add_path> -g <test_golden> -d <image_domain>'
        sys.exit(2)
    
    for opt, arg in opts:
        if opt == '-h':
            print 'combine_results.py -f <file_name> -a <add_path> -g <test_golden> -d <image_domain>'
            sys.exit()
        elif opt in ("-f", "--file_name"):
            fname = arg
        elif opt in ("-a", "--add_path"):
            add = arg
        elif opt in ("-g", "--test_golden"):
            golden = int(arg)
        elif opt in ("-d", "--image_domain"):
            domain = int(arg)

    read(fname, add, golden, domain)


if __name__ == '__main__':
    main(sys.argv[1:])




