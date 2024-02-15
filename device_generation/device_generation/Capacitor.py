import gdspy
from .basic import basic
from .glovar import tsmc40_glovar as glovar
from .Pin import Pin

# Standard Rules from glovar.py
min_w = glovar.min_w
layer = glovar.layer
sp = glovar.sp
en = glovar.en
ex = glovar.ex
NWELL_GR = glovar.NWELL_GR
NW_OD = glovar.NW_OD
NP_OD = glovar.NP_OD
OD_W = glovar.OD_W
GRID = glovar.GRID

# Special Rules for Capacitor
W_CON = 0.14 # Cap contact width
W_VIA = 0.07 # VIA width
SP_VIA = 0.09 # VIA to VIA spacing
EN_VIA = 0.05 # Metal Enclose VIA Rule
VIA_OFF = (W_CON - W_VIA)/2
VIA_DIST = (SP_VIA - W_VIA)/2
SP_SUB = 0.5 # Substrate contact to finger space
W_SUB = min_w['M1'] #0.12 # Sub width

class Capacitor:
    def __init__(self, name, w, sp, nf, l, m_bot=3, m_top=5, attr=[], f_tip=0.14):
        self.name = name
        self.w = w
        self.l = l
        self.sp = sp
        self.nf = nf
        self.metal = []
        self.via = []
        self.m_bot = m_bot
        self.m_top = m_top
        self.plus = Pin('PLUS')
        self.minus = Pin('MINUS')
        self.bulk = Pin('BULK')
        self.t2 = False
        if '2t' in attr:
            self.t2 = True
        # Reversing pin order to top metal first, fix for routing
        for i in range(m_bot, m_top+1):
        #for i in range(m_top, m_bot-1, -1):
            self.metal.append('M'+str(i))
            if i != m_top:
                self.via.append(i)
        self.f_tip = f_tip
        self.cell = gdspy.Cell(name, True)
        self.finger_core()
        if self.t2:
            self.t2_layer()
        else:
            self.lvs_layer()
        self.via_layer()
        self.flatten()
        #self.print_pins()

    def pin(self):
        if self.t2:
            return [self.plus, self.minus]
        else:
            return [self.plus, self.minus, self.bulk]

    def flatten(self):
        if self.origin:
            self.origin = [self.origin[0] + 0.5*min_w['M1'], self.origin[1] + 0.5 * min_w['M1']]
            temp = gdspy.CellReference(self.cell, (-self.origin[0],-self.origin[1]))
            self.cell = gdspy.Cell(self.name, True)
            self.cell.add(temp)
            self.plus.adjust(self.origin)
            self.minus.adjust(self.origin)
            self.bulk.adjust(self.origin)
            self.origin = None
        self.cell.flatten()

    def to_gds(self, multiplier):
        return self.cell.to_gds(multiplier)

    def finger_core(self):
# MOMCAP Finger CORE
    # Define CAP finger shapes
    # This should be legal by default if f_tip=170n, sp=70n, w=70n
        self.W_CON = (basic.legal_len(2*W_CON + 2*self.f_tip + self.l) - 2*self.f_tip - self.l) * 0.5
        self.VIA_OFF = int((self.W_CON - W_VIA)*100)/200.0
        self.finger_y1 = self.W_CON + self.f_tip
        self.finger_y2 = self.finger_y1 + self.l
        self.finger_sp = self.sp + self.w
        con_len = basic.legal_len(self.finger_sp*(self.nf-1)+self.w)
        self.con1 = [[0, 0], [con_len, self.W_CON]]
        self.con2 = [[0, self.finger_y2+self.f_tip], [self.con1[1][0], self.con1[1][1]+self.finger_y2+self.f_tip]]
        self.origin = [0, 0]
        for metal in self.metal:
        # Cap core finger cell
            finger_cell = gdspy.Cell("FINGER", True)
            finger_shape = gdspy.Rectangle((0, self.finger_y1), (self.w, self.finger_y2), layer[metal])
            finger_cell.add(finger_shape)
            finger_array = gdspy.CellArray(finger_cell, self.nf, 1, [self.finger_sp, 0])
            self.cell.add(finger_array)
        # Finger extension
            ext_cell = gdspy.Cell("EXT", True)
            ext_shape = gdspy.Rectangle((0, self.W_CON), (self.w, self.finger_y1), layer[metal])
            ext_cell.add(ext_shape)
            bot_ext = gdspy.CellArray(ext_cell, self.nf/2, 1, [2*self.finger_sp, 0])
            bot_cell = gdspy.Cell("BOT_EXT", True)
            bot_cell.add(bot_ext)
            top_ext = gdspy.CellReference(bot_cell, (self.finger_sp, self.finger_y2-self.W_CON))
            self.cell.add([bot_ext, top_ext])
        # Finger connection metal
            con_bot_shape = gdspy.Rectangle(self.con1[0], self.con1[1], layer[metal])
            con_top_shape = gdspy.Rectangle(self.con2[0], self.con2[1], layer[metal])
            self.cell.add([con_bot_shape, con_top_shape])
        # Adding Pins
            self.plus.add_shape(metal, [self.con1[0],self.con1[1]])
            self.minus.add_shape(metal, [self.con2[0], self.con2[1]])
            #self.plus.add_shape(metal, [self.con1[0],[self.con1[1][0],self.con1[0][1]+min_w['M1']]])
            #self.minus.add_shape(metal, [[self.con2[0][0],self.con2[1][1]-min_w['M1']],self.con2[1]])
        # MOMDMY Layer
            momdmy_shape = gdspy.Rectangle((-GRID, self.W_CON), (self.con2[1][0]+GRID, self.con2[0][1]), layer['MOMDMY'], datatype=int(metal[1]))
            self.cell.add(momdmy_shape)
        #self.flatten()

    def t2_layer(self):
        for metal in self.metal:
        # DMEXCL Layer
            dmexcl_shape = gdspy.Rectangle((0, 0), self.con2[1], layer['DMEXCL'], datatype=int(metal[1]))
            self.cell.add(dmexcl_shape)
    # MOMDMY test0 and dummy8 layer for LVS
    # Layer datatype hard encoded
        shape = gdspy.Rectangle((-GRID, self.W_CON), (self.con2[1][0]+GRID, self.con2[0][1]), layer['MOMDMY'], datatype=100)
        self.cell.add(shape)
        shape = gdspy.Rectangle((-GRID, self.W_CON), (self.con2[1][0]+GRID, self.con2[0][1]), layer['MOMDMY'], datatype=27)
        self.cell.add(shape)
        #self.flatten()

    def lvs_layer(self):
        # Substrate contact
        temp = sp['CO']['CO']
        sp['CO']['CO'] = 0.11
        self.sub_x1 = basic.legal_coord((-SP_SUB-W_SUB,0),(0,0),1)[0]
        self.sub_x2 = basic.legal_coord((self.con2[1][0]+SP_SUB,0),(0,0),2)[0]
        #self.sub_x1 = -SP_SUB-W_SUB
        #self.sub_x2 = self.con2[1][0]+SP_SUB
        for i in [1,2,3,4,5,6]:
            sub_cell = basic.metal_vert(W_SUB, self.con2[1][1],lay=i)
            sub_cell1 = gdspy.CellReference(sub_cell, (self.sub_x1, 0))
            sub_cell2 = gdspy.CellReference(sub_cell, (self.sub_x2, 0))
            self.cell.add(sub_cell1)
            self.cell.add(sub_cell2)
        sp['CO']['CO'] = temp
        self.bulk.add_shape('M6', sub_cell1.get_bounding_box())
        self.bulk.add_shape('M6', sub_cell2.get_bounding_box())
        # PO Dummy Layer
        podmy_shape = gdspy.Rectangle((self.sub_x1, 0), (self.sub_x2+W_SUB, self.con2[1][1]), layer['PO'], datatype=7)
        self.cell.add(podmy_shape)
        # OD25 Layer
        self.od25_p1 = [self.sub_x1-en['OD']['PO'], -en['OD']['PO']]
        self.od25_p2 = [self.sub_x2+en['OD']['PO']+W_SUB, self.con2[1][1]+en['OD']['PO']]
        od25_shape = gdspy.Rectangle(self.od25_p1, self.od25_p2, layer['OD_25'])
        self.cell.add(od25_shape)
        # MOMDMY test0 
        shape = gdspy.Rectangle((-GRID, self.W_CON), (self.con2[1][0]+GRID, self.con2[0][1]), layer['MOMDMY'], datatype=100)
        self.cell.add(shape)
        # MOMDMY dummy2
        shape = gdspy.Rectangle(self.od25_p1, self.od25_p2, layer['MOMDMY'], datatype=21)
        self.cell.add(shape)
        # DMEXCL Layer
        for metal in self.metal:
            dmexcl_shape = gdspy.Rectangle((-GRID, self.W_CON), (self.con2[1][0]+GRID, self.con2[0][1]), layer['DMEXCL'], datatype=int(metal[1]))
            self.cell.add(dmexcl_shape)
        # OD/PO BLK
        shape = gdspy.Rectangle(self.od25_p1, self.od25_p2, layer['DMEXCL'], datatype=20)
        self.cell.add(shape)
        shape = gdspy.Rectangle(self.od25_p1, self.od25_p2, layer['DMEXCL'], datatype=21)
        self.cell.add(shape)
        #self.flatten()


    def via_layer(self):
        width = self.con1[1][0]
        via_count = int((width - 2 * EN_VIA + SP_VIA)/(W_VIA + SP_VIA))
        via_offset = (width - W_VIA - (SP_VIA + W_VIA) * (via_count - 1))/2
        delta = 0.5 * (SP_VIA + W_VIA)
        for via in self.via:
            via_cell = gdspy.Cell("VIA", True)
            via_layer = layer['VIA'+str(via)]
            via_shape = gdspy.Rectangle((0, self.VIA_OFF), (W_VIA, self.VIA_OFF+W_VIA), via_layer)
            via_cell.add(via_shape)
            if (via - self.m_bot) % 2 == 0:
                via_array1 = gdspy.CellArray(via_cell, via_count, 1, [SP_VIA+W_VIA, 0], (via_offset, 0))
                via_array2 = gdspy.CellArray(via_cell, via_count, 1, [SP_VIA+W_VIA, 0], (via_offset, self.con2[0][1]))
            else:
                via_array1 = gdspy.CellArray(via_cell, via_count-1, 1, [SP_VIA+W_VIA, 0], (via_offset+delta, 0))
                via_array2 = gdspy.CellArray(via_cell, via_count-1, 1, [SP_VIA+W_VIA, 0], (via_offset+delta, self.con2[0][1]))
            self.cell.add(via_array1)
            self.cell.add(via_array2)
        #self.flatten()

    def print_pins(self):
        if not (self.plus.check() and self.minus.check() and self.bulk.check()):
            print("Pin location not legal")
        if self.t2:
            pass
            #print self.plus, self.minus
        else:
            self.bulk.check()
            #print self.plus, self.minus, self.bulk

    def flip_vert(self):
        flip_cell = gdspy.Cell(self.cell.name, True)
        bounding_box = self.cell.get_bounding_box()
        x_sym_axis = bounding_box[0][0] + bounding_box[1][0]
        # Floating point error 
        # Since gdsii precision is 5nm, here we only round to 1nm precision
        #x_sym_axis = round(x_sym_axis * 10000) / 10000.0
        polydict = self.cell.get_polygons(by_spec=True)
        for key in polydict:
            layer, datatype = key
            for shape in polydict[key]:
                x_min = shape[0][0]
                y_min = shape[0][1]
                x_max = shape[2][0]
                y_max = shape[2][1]
                x_min_s = x_sym_axis - x_max
                x_max_s = x_sym_axis - x_min
                new_shape = gdspy.Rectangle([x_min_s,y_min], [x_max_s,y_max], layer, datatype=datatype)
                flip_cell.add(new_shape)
        self.cell = flip_cell
        #self.flatten()
        self.plus.flip_vert(x_sym_axis)
        self.minus.flip_vert(x_sym_axis)
        self.bulk.flip_vert(x_sym_axis)

    def bounding_box(self):
        if self.origin:
            assert self.origin == [0, 0], "Cell origin not reset."
        bounding_box = self.cell.get_bounding_box()
        ll = list(basic.legal_coord(bounding_box[0],[0,0],1))
        ur = list(basic.legal_coord(bounding_box[1],[0,0],3))
        ll[0] = ll[0] - min_w['M1'] - min_w['SP']
        ll[1] = ll[1] - min_w['M1'] - min_w['SP']
        ur[0] = ur[0] + min_w['M1'] + min_w['SP']
        ur[1] = ur[1] + min_w['M1'] + min_w['SP']
        return [ll,ur]
