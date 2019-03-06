import os
import sys, getopt
import numpy as np
import gdspy
import shutil
import pdb
import math
from PIL import Image, ImageDraw, ImageOps,ImageFilter

#add='/home/polaris/anoynmousanoynmous/Documents/Projects/utda-toshiba/SRAF_insertion/SRAF_xiaoqing/gds/initial/MB_SRAF_clips/'
#add='/home/local/eda03/anoynmousanoynmous/documents/Projects/resistmodel/results/N10/'

##save the coord in a list of lists

def read(infile,add='', desired_size=256):
    # print(infile)
    # scale = 0.02 # 0.012  this number is used to scale the image to 256*256-> it depends on the original input size
    # scale = 0.03 # 0.012  this number is used to scale the image to 256*256-> it depends on the original input size
    scale = 0.06 # 0.012  this number is used to scale the image to 256*256-> it depends on the original input size
    gdsii = gdspy.GdsLibrary()          # create a gpspy library
    gdsii.read_gds(add+'gdsii/'+infile) # read gds file and rename to avoid using the same name->. "TOP" is the original cell name
    print "length of top cells: ", len(gdsii.top_level())
    cell = gdsii.top_level()[0]
    print cell.name
    inPolygons = cell.get_polygons(True)            # read polygons-> inPolygons is a dictionary of arrays index by the layer number 
    bbox_x = cell.get_bounding_box()[:,0]   # get the bounding box of the design
    bbox_y = cell.get_bounding_box()[:,1]
    print "Lower-left corner: (", bbox_x[0], ",", bbox_y[0], ")"
    print "Upper-right corner: (", bbox_x[1], ",", bbox_y[1], ")"
    width  = int((bbox_x[1] - bbox_x[0])/scale)
    height = int((bbox_y[1] - bbox_y[0])/scale)
    img_b = Image.new('RGB', (width, height), 0) 
    img_g = Image.new('RGB', (width, height), 0) 
    img_r = Image.new('RGB', (width, height), 0) 
    # draw N well
    if (3,0) in inPolygons:                      # this is only to check if the layer we are interested in is present in the gds
    	for pp in inPolygons[(3,0)]:             # read all polygons corresponding to the desired layer (2010) in this case
    	    pa = np.array(pp)
    	    pa[:,0] -= bbox_x[0]                # the image is recentered so the location of the polygon is adjusted based on the bounding box location
    	    pa[:,1] -= bbox_y[0]
    	    pa = (pa/scale).astype(int)         # scale the polygon and map it to int (pixel)
    	    ImageDraw.Draw(img_b).polygon(pa.ravel().tolist(), outanoynmouse=(0,0,255), fill=(0,0,255)) #draw the polygon on the image . . fill (R,G,B) is the color scheme . outanoynmouse is similar
    # draw OD outside N well, green 
    if (6,0) in inPolygons:                     
    	for pp in inPolygons[(6,0)]:             
    	    pa = np.array(pp)
    	    pa[:,0] -= bbox_x[0]               
    	    pa[:,1] -= bbox_y[0]
    	    pa = (pa/scale).astype(int)         
    	    ImageDraw.Draw(img_g).polygon(pa.ravel().tolist(), outanoynmouse=(0,255,0), fill=(0,255,0)) 
    # draw OD inside N well, red
    if (6,1) in inPolygons:                     
    	for pp in inPolygons[(6,1)]:             
    	    pa = np.array(pp)
    	    pa[:,0] -= bbox_x[0]               
    	    pa[:,1] -= bbox_y[0]
    	    pa = (pa/scale).astype(int)         
    	    ImageDraw.Draw(img_r).polygon(pa.ravel().tolist(), outanoynmouse=(255,0,0), fill=(255,0,0)) 
    
    # R, G, B = img_r.getdata(0), img_g.getdata(1), img_b.getdata(2)
    R, _, _ = img_r.split()
    _, G, _ = img_g.split()
    _placeholder, _, B = img_b.split()
    img_d2 = Image.merge('RGB', [R, G, B]) 
    img_d1 = Image.merge('RGB', [R, G, _placeholder]) # blue channel is 0 

    # clip image
    print "width, height: ", width, ", ", height
    num_row = height/desired_size + 1
    num_col = width/desired_size + 1
    print "num_row, num_col: ", num_row, ", ", num_col
    for i in range(num_row):
        for j in range(num_col):
            sub_img_merged = Image.new('RGB', (2*desired_size, desired_size), 0)
            # sub_img_d2 = Image.new('RGB', (desired_size, desired_size), 0)
            # sub_img_d1 = Image.new('RGB', (desired_size, desired_size), 0)
            crop_width = desired_size*(j+1)
            crop_height = desired_size*(i+1)
            if crop_width > width:
                crop_width = width
            if crop_height > height:
                crop_height = height
            # crop_rectangle = (desired_size*j, desired_size*i, desired_size*(j+1)-1, desired_size*(i+1)-1)
            crop_rectangle = (desired_size*j, desired_size*i, crop_width, crop_height)
            sub_img_d2 = img_d2.crop(crop_rectangle) 
            sub_img_d1 = img_d1.crop(crop_rectangle) 
            if sub_img_d1.getbbox():
                sub_img_merged.paste(sub_img_d2, (0, 0))
                sub_img_merged.paste(sub_img_d1, (desired_size, 0))
                sub_img_merged.save(add+'merged_images/'+cell.name+'_'+str(i)+'_'+str(j)+'.png')
    img_d2.save(add+'images/'+infile+'.png')
		

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "hf:a:", ["file_name=", "add_path="])
    except getopt.GetoptError:
        print 'parser_v2.py -f <file_name> -a <add_path>'
        sys.exit(2)
    
    for opt, arg in opts:
        if opt == '-h':
            print 'parser_v2.py -f <file_name> -a <add_path>'
            sys.exit()
        elif opt in ("-f", "--file_name"):
            fname = arg
        elif opt in ("-a", "--add_path"):
            add = arg

    # add="/home/local/eda06/anoynmousanoynmous/Research/idea_place/WellGen/DataWellGen2/"
    read(fname, add)


if __name__ == '__main__':
    main(sys.argv[1:])




