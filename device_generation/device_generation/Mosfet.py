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
SUB_GR = glovar.SUB_GR
KR_SP = glovar.KR_SP

# Special rulse related with gate spacing
dummy_l = 0.06 #critical length for dummy poly
crit_l = 0.12 #critical length for poly that require additional spacing 
crit_sp = 0.16 #critical additional spacing for PO

# Special rules for OD_25 related rules
gate_space_25 = 0.22
sp_co_po_25 = 0.08
en_od_25 = 0.2  # Enclosue rule of OD_25 and OD layer

# Special rules for NT_N related rules
ex_po_od_na = 0.35

class Mosfet:
# Currently supported Mosfet types: nch, pch
# Supported attribute keywords: od25, ud, od, na, lvt, hvt, mac
# Note: na devices should only be compatible with nch
# Unsupported attributes: x, dnw, hv25, snw, na25
# GATE Connection metal check legalization if min_w['M1'] not 0.07
    def __init__(self, nch, name, w, l, nf, attr=[], spectre=True, pinConType=None, bulkCon=[]):
        self.nmos = nch
        self.name = name
        if spectre:
            self.w = w/nf
        else:
            self.w = w 
        self.l = l
        self.nf = nf
        self.od25 = False
        self.ud18 = False
        self.od33 = False
        self.drain = Pin('D')
        self.gate = Pin('G')
        self.source = Pin('S')
        self.bulk = Pin('B')
        self.cell = gdspy.Cell(name, True)
        if '25' in attr:
            sp_co_po = sp['CO']['PO']
            self.od25 = True
            sp['CO']['PO'] = sp_co_po_25
        if 'ud' in attr:
            self.ud18 = True
        if 'od' in attr:
            self.od33 = True
        if 'na' in attr:
            ex_po_od = ex['PO']['OD']
            ex['PO']['OD'] = ex_po_od_na
        self.mos_core()
        self.doping_layer()
        if 'lvt' in attr:
            self.vth_layer(True)
        if 'hvt' in attr:
            self.vth_layer(False)
        if 'mac' in attr:
            self.mac_layer()
        if 'na' in attr:
            self.na_layer()
            ex['PO']['OD'] = ex_po_od
        if '25' in attr:
            self.od25_layer()
            sp['CO']['PO'] = sp_co_po
        self.bulkCon = bulkCon
        self.nwell_gr()
        self.connect_pin(pinConType)
        self.connect_pin_bulk()
        self.flatten()
        self.print_pins()

    def pin(self):
        # TODO Future version should include all shapes
        if not self.nmos and NWELL_GR:
            return [self.drain, self.gate, self.source, self.bulk]
        else:
            return [self.drain, self.gate, self.source]
       
    def flatten(self):
        if self.origin:
            self.origin = [self.origin[0] + 0.5*min_w['M1'], self.origin[1] + 0.5 * min_w['M1']]
            temp = gdspy.CellReference(self.cell, (-self.origin[0],-self.origin[1]))
            self.cell = gdspy.Cell(self.name, True)
            self.cell.add(temp)
            self.drain.adjust(self.origin)
            self.gate.adjust(self.origin)
            self.source.adjust(self.origin)
            self.bulk.adjust(self.origin)
            self.origin = None
        self.cell.flatten()

# Compatible with gdspy
    def to_gds(self, multiplier):
        #self.flatten()
        return self.cell.to_gds(multiplier)

    def vth_layer(self, lvt):
        if lvt:
            if self.nmos:
                vth_layer = layer['VTL_N']
            else:
                vth_layer = layer['VTL_P']
        else:
            if self.nmos:
                vth_layer = layer['VTH_N']
            else:
                vth_layer = layer['VTH_P']
        vth_shape = gdspy.Rectangle(self.nw_ll, self.nw_ur, vth_layer)
        self.cell.add(vth_shape)
        #self.flatten()    
    
    def mac_layer(self):
        mac_shape = gdspy.Rectangle(self.nw_ll, self.nw_ur, layer['LVSDMY'], datatype=1)
        self.cell.add(mac_shape)
        #self.flatten()

    def od25_layer(self):
        od25_p1 = [self.x_od1-en_od_25, -en_od_25]
        od25_p2 = [self.x_od2+en_od_25, self.w+en_od_25]
        od25_shape = gdspy.Rectangle(od25_p1, od25_p2, layer['OD_25'])
        self.cell.add(od25_shape)
        if self.ud18:
            od_25ud_shape = gdspy.Rectangle(od25_p1, od25_p2, layer['OD_25'], datatype=4)
            self.cell.add(od_25ud_shape)
        elif self.od33:
            od_25od_shape = gdspy.Rectangle(od25_p1, od25_p2, layer['OD_25'], datatype=3)
            self.cell.add(od_25od_shape)
        #self.flatten()

    def na_layer(self):
        na_p1 = [self.x_od1-en['NT_N']['OD'], -en['NT_N']['OD']]
        na_p2 = [self.x_od2+en['NT_N']['OD'], self.w+en['NT_N']['OD']]
        na_shape = gdspy.Rectangle(na_p1, na_p2, layer['NT_N'])    
        self.cell.add(na_shape)
        #self.flatten()

    def mos_core(self):
        self.nf_change = False
    ### Source Drain Metal Contact
        m1_cell = basic.metal_vert(min_w['M1'], self.w)
        m1_y_legal = -basic.legal_len(self.w) + self.w
        m1_legal_shape = gdspy.Rectangle((0,m1_y_legal), (min_w['M1'], self.w), layer['M1'])
        m1_cell.add(m1_legal_shape)
        m1_cell_space = basic.legal(self.l + min_w['CO'] + 2 * sp['CO']['PO'])
        if self.l > crit_l: #handling special rule 
            m1_cell_space = basic.legal(crit_sp + self.l)
        if self.od25:
            m1_cell_space =  basic.legal(gate_space_25 + self.l)
        m1_array_offset = self.l + 0.5*(m1_cell_space - self.l - min_w['M1']) - m1_cell_space
        m1_array = gdspy.CellArray(m1_cell, (self.nf+1), 1, [m1_cell_space, 0], [m1_array_offset, 0])
        self.cell.add(m1_array)
        self.origin = [m1_array_offset, m1_y_legal]
    ### Gate Poly
        gate_cell = gdspy.Cell('GATE', True)
        gate_shape = gdspy.Rectangle((0, -ex['PO']['OD']), (self.l, self.w+ex['PO']['OD']), layer['PO'])
        gate_cell.add(gate_shape)
        self.gate_space = m1_cell_space
        gate_array = gdspy.CellArray(gate_cell, self.nf, 1, [self.gate_space, 0])
        self.cell.add(gate_array)
    ### Dummy Gate
        if self.l < dummy_l:
            dummy1_shape = gdspy.Rectangle((-self.gate_space, 0), (-self.gate_space+self.l, self.w), layer['PO'])
            self.cell.add(dummy1_shape)
            dummy2_shape = gdspy.Rectangle((self.nf*self.gate_space, 0), (self.nf*self.gate_space+self.l, self.w), layer['PO'])
            self.cell.add(dummy2_shape)
    ### OD Layer
        self.x_od1 = -(self.gate_space - min_w['M1'] - self.l)/2 - min_w['CO'] - 0.5*(min_w['M1'] - min_w['CO']) - en['OD']['CO']
        if self.nf % 2 == 0 or self.nf == 1:
            self.x_od2 = (self.nf-1)*self.gate_space + self.l - self.x_od1
            od_shape = gdspy.Rectangle((self.x_od1, 0), (self.x_od2, self.w), layer['OD'])
        else:
            self.x_od2 = self.nf*self.gate_space + self.l - self.x_od1
            od_shape = gdspy.Rectangle((self.x_od1, 0), (self.x_od2 - self.gate_space, self.w), layer['OD'])
        self.cell.add(od_shape)
    ### GATE Connection
        # Single Finger, no need to connect source/drain
        if self.nf == 1: 
            # Gate Contact
            width = min_w['CO'] + 2 * en['M1']['CO']
            self.gate_ext_len = min_w['SP']
            #self.gate_ext_len = basic.legal(ex['PO']['OD'] + 0.5 * (min_w['CO'] + 2*en['PO']['CO'] - min_w['M1'])) - min_w['M1']
            if width >= self.l:
                x_pos = 0.5*(width-self.l) 
                y_pos = self.w + self.gate_ext_len - 0.5 * (min_w['CO'] + 2*en['PO']['CO'] - min_w['M1'])
                # TODO: increase PO_W for potential DRC violations here
                gate_con_shape = gdspy.Rectangle((-x_pos, y_pos), (-x_pos+width, y_pos+min_w['CO']+2*en['PO']['CO']), layer['PO'])
                self.cell.add(gate_con_shape)
                # Legalization added con_shape
                gate_con_shape = gdspy.Rectangle((0, self.w), (self.l, y_pos), layer['PO'])
                self.cell.add(gate_con_shape)
                # Legalization for top metal
                y_pos = y_pos + en['PO']['CO']-0.5*(min_w['M1']-min_w['CO'])
                #x_pos_legal1 = basic.legal_coord((-x_pos, y_pos),self.origin,1)[0]
                #x_pos_legal2 = basic.legal_coord((-x_pos+width, y_pos+min_w['M1']),self.origin,3)[0]
                x_pos_legal1 = m1_array_offset
                x_pos_legal2 = m1_array_offset + self.gate_space 
                gate_m1_shape = gdspy.Rectangle((x_pos_legal1, y_pos),(x_pos_legal2+min_w['M1'], y_pos+min_w['M1']), layer['M1'])
                self.cell.add(gate_m1_shape)
                x_pos = -x_pos + 0.5*(width-min_w['CO'])
                y_pos = y_pos + 0.5*(min_w['M1']-min_w['CO'])
                gate_co_shape = gdspy.Rectangle((x_pos, y_pos), (x_pos+min_w['CO'], y_pos+min_w['CO']), layer['CO'])
                self.cell.add(gate_co_shape)
                # Adding Pin:G
                self.gate.add_shape('M1', gate_m1_shape.get_bounding_box())
            else:
                y_pos = self.w + self.gate_ext_len + 0.5*(min_w['M1']+min_w['CO']) + en['PO']['CO']
                gate_con_shape = gdspy.Rectangle((0, self.w), (self.l, y_pos), layer['PO'])
                self.cell.add(gate_con_shape)
                y_pos = self.w + self.gate_ext_len
                m1_gate = basic.metal_hori(self.l, min_w['M1'])
                m1_gate_ref = gdspy.CellReference(m1_gate, (0, y_pos))
                self.cell.add(m1_gate_ref)
                # Legalization M1
                x_pos_legal1 = m1_array_offset
                x_pos_legal2 = m1_array_offset + self.gate_space 
                gate_m1_shape = gdspy.Rectangle((x_pos_legal1, y_pos),(x_pos_legal2+min_w['M1'], y_pos+min_w['M1']), layer['M1'])
                self.cell.add(gate_m1_shape)
                # Adding Pin:G
                self.gate.add_shape('M1', gate_m1_shape.get_bounding_box())
        # Multiple Finger, need to connect source/drain
        else:
            # Source/Drain M1 Conection
            # KR_SP feature has been removed
            assert KR_SP == 0, "KR_SP FEATURE HAS BEEN REMOVED"
            # Gate Connection
            gate_extension = gdspy.Cell('GATE_EXT', True)
            # Modified for legal
            self.gate_ext_len = 2 * min_w['SP'] + min_w['M1']#+ 1 * sp['M1']['M1'] + KR_SP
            if ex['PO']['OD'] > self.gate_ext_len:  # Handling nch_na devices
                self.gate_ext_len = basic.legal(ex['PO']['OD'] - (min_w['CO'] + 2*en['PO']['CO'])) + min_w['M1']#+ 0.5 * (min_w['CO'] + 2*en['PO']['CO'] - min_w['M1']) + KR_SP) - min_w['M1']
            # REMOVED ELSE CLAUSE
            #else: # Need to extend gate shape
            gate_ext_shape = gdspy.Rectangle((0, self.w), (self.l, self.w+self.gate_ext_len), layer['PO'])
            gate_extension.add(gate_ext_shape)
            gate_extension_array = gdspy.CellArray(gate_extension, self.nf, 1, [self.gate_space, self.gate_space])
            self.cell.add(gate_extension_array)
            #end else
            y_pos = self.w + self.gate_ext_len - ( en['PO']['CO'] - 0.5 * ( min_w['M1'] - min_w['CO'] ) )
            gate_hori_shape = gdspy.Rectangle((0, y_pos), (self.gate_space*(self.nf-1)+self.l, y_pos+min_w['CO']+2*en['PO']['CO']), layer['PO'])
            self.cell.add(gate_hori_shape)
            gate_contact = basic.metal_hori(self.gate_space*(self.nf-1)+self.l, min_w['M1'])
            gate_contact_ref = gdspy.CellReference(gate_contact, (0, self.w+self.gate_ext_len))
            self.cell.add(gate_contact_ref)
            # Generate symmetry source connections
            # self.nf changes here!!!
            if self.nf % 2 == 1:
                self.nf_change = True
                self.nf += 1
                m1_dummy_x = m1_array_offset + m1_cell_space * self.nf
                m1_dummy_source = gdspy.Rectangle((m1_dummy_x, m1_y_legal), (m1_dummy_x+min_w['M1'],self.w), layer['M1'])
                self.cell.add(m1_dummy_source)
            m1_square = gdspy.Cell('M1_SQUARE', True)
            m1_sq_shape = gdspy.Rectangle((0, -KR_SP), (min_w['M1'], KR_SP+min_w['SP']), layer['M1'])
            m1_square.add(m1_sq_shape)
            source_count = int(self.nf/2+1)
            drain_count = int((self.nf+1)/2)
            m1_source = gdspy.CellArray(m1_square, source_count, 1, [2*self.gate_space, self.gate_space], [m1_array_offset, self.w])
            m1_drain = gdspy.CellArray(m1_square, drain_count, 1, [2*self.gate_space, self.gate_space], [m1_array_offset+self.gate_space, -min_w['SP']+self.origin[1]])
            m1_source_hori = gdspy.Rectangle((m1_array_offset, self.w+KR_SP+min_w['SP']), 
                (m1_array_offset+(2*source_count-2)*self.gate_space+min_w['M1'], self.w+KR_SP+min_w['SP']+min_w['M1']), layer['M1'])
            m1_drain_hori = gdspy.Rectangle((m1_array_offset+self.gate_space, self.origin[1]-min_w['SP']-min_w['M1']), 
                (m1_array_offset+(2*drain_count-1)*self.gate_space+min_w['M1'], self.origin[1]-min_w['SP']), layer['M1'])
            self.cell.add(m1_source)
            self.cell.add(m1_source_hori)
            if drain_count > 1:
                self.cell.add(m1_drain)
                self.cell.add(m1_drain_hori)
            # Legalization M1 Gate
            y_pos = self.w + self.gate_ext_len
            x_pos_legal1 = m1_array_offset
            x_pos_legal2 = m1_array_offset + self.nf*self.gate_space 
            gate_m1_shape = gdspy.Rectangle((x_pos_legal1, y_pos),(x_pos_legal2+min_w['M1'], y_pos+min_w['M1']), layer['M1'])
            self.cell.add(gate_m1_shape)
            # Adding Pin:G
            self.gate.add_shape('M1', gate_m1_shape.get_bounding_box())
        # Adding Pin:S/D
        if self.nf == 1:
            self.source.add_shape('M1', [[m1_array_offset, self.origin[1]], [m1_array_offset+min_w['M1'], self.w]])
            self.drain.add_shape('M1', [[m1_array_offset+self.gate_space, self.origin[1]], [m1_array_offset+min_w['M1']+self.gate_space, self.w]])
        #elif self.nf == 2:
        #    self.source.add_shape('M1', m1_source_hori.get_bounding_box())
        #    self.drain.add_shape('M1', [[m1_array_offset+self.gate_space, -2*min_w['M1']], [m1_array_offset+min_w['M1']+self.gate_space, self.w]])
        else:
            self.source.add_shape('M1', m1_source_hori.get_bounding_box())
            if drain_count > 1:
                self.drain.add_shape('M1', m1_drain_hori.get_bounding_box())
            for i in range(self.nf+1):
                if i % 2 == 0:
                    self.source.add_shape('M1', [[self.origin[0]+i*self.gate_space, self.origin[1]], [self.origin[0]+i*self.gate_space+min_w['M1'], self.w]])
                else:
                    self.drain.add_shape('M1', [[self.origin[0]+i*self.gate_space, self.origin[1]], [self.origin[0]+i*self.gate_space+min_w['M1'], self.w]])
        #self.flatten()
    
    def nwell_gr(self):
        if not self.nmos and NWELL_GR:
            nwell_gr, self.bulk = basic.nwell_GR(self.cell.get_bounding_box()[0], self.cell.get_bounding_box()[1],self.origin, (1 not in self.bulkCon))    
            #self.add_bulk_shape(nwell_gr.get_polygons(True)[(layer['M1'],0)])
            nwell_gr_ref = gdspy.CellReference(nwell_gr)
            self.cell.add(nwell_gr_ref)
            #self.flatten()
        if self.nmos and SUB_GR:
            sub_gr, self.bulk = basic.sub_GR(self.cell.get_bounding_box()[0], self.cell.get_bounding_box()[1],self.origin, (1 not in self.bulkCon))    
            #self.add_bulk_shape(sub_gr.get_polygons(True)[(layer['M1'],0)])
            sub_gr_ref = gdspy.CellReference(sub_gr)
            self.cell.add(sub_gr_ref)
            #self.flatten()

    def add_bulk_shape(self, shapes):
        # TODO future should include all shapes
        shape = shapes[3] # This is the vertical top strip of M1
        self.bulk.add_shape('M1', [shape[0], shape[2]])

    def doping_layer(self):
        if self.nmos:
            doping_layer = layer['NP']
        else:
            doping_layer = layer['PP']
    # Define NP/PP and NW Shapes
        if self.l < dummy_l:
            self.dope_ll = [-self.gate_space-en['NP']['PO'], -ex['PO']['OD']-en['NP']['PO']]
            self.dope_ur = [self.nf*self.gate_space+self.l+en['NP']['PO'], self.w+min_w['M1']+self.gate_ext_len+en['PO']['CO']-0.5*(min_w['M1']-min_w['CO'])+en['NP']['PO']] 
            self.nw_ll = [-self.gate_space-en['PP']['PO'], -en['NW']['OD'][1]]
            self.nw_ur = [self.nf*self.gate_space+self.l+en['PP']['PO'], self.w+en['NW']['OD'][1]] 
        else:
            self.dope_ll = [self.x_od1-ex['NP']['OD'], -ex['PO']['OD']-en['NP']['PO']]
            self.dope_ur = [self.x_od2+ex['NP']['OD'], self.w+min_w['M1']+self.gate_ext_len+en['PO']['CO']-0.5*(min_w['M1']-min_w['CO'])+en['NP']['PO']]
            self.nw_ll = [self.x_od1-en['NW']['OD'][0], -en['NW']['OD'][1]]
            self.nw_ur = [self.x_od2+en['NW']['OD'][0], self.w+en['NW']['OD'][1]]
        # Shrink dope due to self.nf change
        # Draw NP/PP
        doping_shape = gdspy.Rectangle(self.dope_ll, self.dope_ur, doping_layer)
        self.cell.add(doping_shape)
        # For PMOS
        if not self.nmos:
            # Draw NW
            nw_shape = gdspy.Rectangle(self.nw_ll, self.nw_ur, layer['NW'])
            self.cell.add(nw_shape)
        #self.flatten()

    def print_pins(self):
        if not (self.drain.check() and self.gate.check() and self.source.check() and self.bulk.check()):
            print("Pin location not legal")
        #print self.drain, self.gate, self.source, self.bulk

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
        self.drain.flip_vert(x_sym_axis)
        self.gate.flip_vert(x_sym_axis)
        self.source.flip_vert(x_sym_axis)
        self.bulk.flip_vert(x_sym_axis)

    def connect_pin_bulk(self):
        # This is only valid for device with guard ring
        if 1 in self.bulkCon:
            _, ll, ur = self.gate.shape[0]
            _, _, ur_bulk = self.bulk.shape[1]
            con1 = gdspy.Rectangle((ll[0], ur_bulk[1]-min_w['M1']),(ll[0]+min_w['M1'], ll[1]), layer['M1'])
            con2 = gdspy.Rectangle((ur[0]-min_w['M1'], ur_bulk[1]-min_w['M1']),(ur[0], ll[1]), layer['M1'])
            self.cell.add([con2])#,con2])
        if 2 in self.bulkCon:
            _, ll, _ = self.source.shape[0]
            _, _, ur_bulk = self.bulk.shape[0]
            con = gdspy.Rectangle((ll[0], ur_bulk[1]-min_w['M1']),(ll[0]+min_w['M1'], ll[1]), layer['M1'])
            self.cell.add(con)
        if 0 in self.bulkCon:
            _, _, ur = self.drain.shape[0]
            _, _, ur_bulk = self.bulk.shape[0]
            con = gdspy.Rectangle((ur[0]-min_w['M1'], ur_bulk[1]-min_w['M1']),(ur[0], ur[1]), layer['M1'])
            self.cell.add(con)

    def connect_pin(self, conType):
        # conType, 0: G,S; 1: G,D; 2: S,D;
        # special case G,D; S,B; call connect_pin then connect_pin_bulk
        if conType in ['GS','SG']:
            _, ll, _ = self.source.shape[0]
            _, _, ur = self.gate.shape[0]
            x = self.origin[0] + (self.nf / 2) * self.gate_space
            con = gdspy.Rectangle((x, ll[1]),(x+min_w['M1'], ur[1]), layer['M1'])
            self.cell.add(con)
        elif conType in ['GD','DG']: 
            _, ll, _ = self.source.shape[0]
            _, _, ur = self.gate.shape[0]
            x = self.origin[0] + (self.nf / 2) * self.gate_space
            con = gdspy.Rectangle((x, ll[1]),(x+min_w['M1'], ur[1]), layer['M1'])
            self.cell.add(con)
            # swap source/drain
            tempPin = self.source
            self.source = self.drain
            self.drain = tempPin
        elif conType in ['SD','DS']:
            if self.nf == 1:
                _, ll, _ = self.source.shape[0]
                _, _, ur = self.drain.shape[0]
                con1 = gdspy.Rectangle((ll[0], ll[1]),(ur[0], ll[1]+min_w['M1']), layer['M1'])
                con2 = gdspy.Rectangle((ll[0], ur[1]-min_w['M1']),(ur[0], ur[1]), layer['M1'])
                self.cell.add([con2])#,con2])
            else:
                _, ll, _ = self.source.shape[1]
                _, _, ur = self.source.shape[-1]
                con = gdspy.Rectangle((ll[0], ll[1]),(ur[0], ll[1]+min_w['M1']), layer['M1'])
                self.cell.add(con)

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
