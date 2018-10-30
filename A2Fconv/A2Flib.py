#!/usr/bin/env python

from A2Fstruct import *
import math
import re

## class definition ##
class abqST:
    def __init__(self):
        self.NSETS = {} #NSETS[nset] = [[GENERATE(T/F),DATALINES([str])]]
        self.ELSETS = {} #ELSETS[elset] = [GENERATE(T/F),DATALINES([str])]
        self.SOLID_SECTIONS = [] #[{ELSET(str),MATERIAL(str),ORIENTATION(str)}]
        self.SURFACES = {} #SURFACES[name] = [TYPE(str),DATALINES([str])]
        self.MATERIALS = {} #MATERIALS[name] = {PROP(str):{OPT,DATELINES}}
        self.INT_PROPARTIES = {} #INT_PROPATIES[name] = {Fric:DATALINES[str]}
        self.CONTACT_PAIRS = [] #{INTPROP,SLID_TYPE,SLAVE,MASTER}
        self.BOUNDARIES = [] #[DATALINES([str])]
        self.LOADS = [] #[{TYPE(str):DATALINES([str])}]
        self.INITIAL_CONDITION = {} #{TYPE(str),DATALINE}
        self.TEMPERATURE = [] #[DATALINES([str])]
        self.STEPS = [{}] #[{TYPE(str),ID([int])}]
        self.SPRINGS = {} #[{ELSET(str):DATALINES([str])}]
        self.ORIENTATION = {} #ORIENTATION[name]={DEFINITION(str),SYSTEM(str),DATALINES([str])}
        self.MPC = [] #[{SLAVE(str),MASTER(str)}]
        self.AMP = {} #AMP[name] = {VALUE[str],DATALINES[str[]]}
        self.FIELDOUTPUT = [] #[{NOUT,EOUT,freq,}]
        self.curr_mat = "" #current material
        self.curr_spring = "" #current material
        self.curr_intprop = "" #current interaction property
        self.curr_slidtype = ""
        self.curr_positiontol = ""
        self.curr_adjust = "NO"
        self.curr_boundary = -1
        self.curr_load = -1
        self.curr_temp = -1
        self.curr_step = 0
        self.curr_ori = ''
        self.curr_fout = -1
        self.nlgeom = False
        self.eigen = False
        self.freq_modal = False

    def printST(self):
        print "### NSET         ###"
        for k in self.NSETS.keys(): print k,self.NSETS[k]
        print "### ELSET        ###"
        for k in self.ELSETS.keys(): print k,self.ELSETS[k]
        print "### SOLID SECTION###"
        for i in self.SOLID_SECTIONS: print i
        print "### SURFACE      ###"
        for k in self.SURFACES.keys(): print k,self.SURFACES[k]
        print "### MATERIAL     ###"
        for k in self.MATERIALS.keys(): print k,self.MATERIALS[k]
        print "###INT_PROPARTY ###"
        for k in self.INT_PROPARTIES.keys(): print k,self.INT_PROPARTIES[k]
        print "###CONTACT PAIR ###"
        for i in self.CONTACT_PAIRS: print i
        print "###BOUNDARY     ###"
        for i in self.BOUNDARIES: print i
        print "###LOAD         ###"
        for i in self.LOADS: print i
        print "###INITIAL COND.###",self.INITIAL_CONDITION
        print "###TEMPERATURE  ###",self.TEMPERATURE
        print "###STEPS        ###",self.STEPS
        print "###NLGEOM       ###",self.nlgeom
        print "\n"

class fstrST:
    def __init__(self):
        self.NGROUPS = {} #NSETS[nset] = [GENERATE(T/F),DATALINES([str])]
        self.EGROUPS = {} #ELSETS[elset] = [GENERATE(T/F),DATALINES([str])]
        self.SECTIONS = [] #[{ELSET(str),MATERIAL(str),ORIENTATION(str)}]
        self.SGROUPS = {} #SURFACES[name] = [TYPE(str),DATALINES([str])]
        self.MATERIALS = {} #MATERIALS[name] = {PROP(str):DATALINES([str])}
        self.CONTACT_PAIRS = [] #{FRIC,SLID_TYPE,SLAVE,MASTER}
        self.BOUNDARIES = [] #[NGRP,DIRECTION(str),val(str)]
        self.LOADS = [] #[{TYPE(str):DATALINES([str])}]
        self.INITIAL_CONDITION = {} #{TYPE(str),DATALINE}
        self.TEMPERATURE = [] #[DATALINES([str])]
        self.STEPS = [{}] #[{TYPE(str),ID(int)}]
        self.ORIENTATOIN = {} #ORIENTATION[name]={DEFINITION(str),SYSTEM(str),DATALINES([str])}
        self.ROTINFO = [] # dict, key=BOUNDARY, CLOAD, MPC
        self.AMP = {} #AMP[name] = {VALUE[str],DATALINES[str[]]}
        self.nlstatic = False
        self.eigen = False
        self.freq_modal = False
        self.REFTEMP = ''
        self.OUTPUT = {}
        self.VTK_OUTPUT = []

    def printST(self):
        print "### NGROUP       ###"
        for k in self.NGROUPS.keys(): print k,self.NGROUPS[k]
        print "### EGROUP       ###"
        for k in self.EGROUPS.keys(): print k,self.EGROUPS[k]
        print "### SECTIONS     ###"
        for i in self.SECTIONS: print i
        print "### SGROUP       ###"
        for k in self.SGROUPS.keys(): print k,self.SGROUPS[k]
        print "### MATERIAL     ###"
        for k in self.MATERIALS.keys(): print k,self.MATERIALS[k]
        print "### CONTACT PAIR ###"
        print "CONTACT PAIR :",self.CONTACT_PAIRS
        print "BOUNDARY     :",self.BOUNDARIES
        print "LOAD         :",self.LOADS
        print "INITIAL COND.:",self.INITIAL_CONDITION
        print "TEMPERATURE  :",self.TEMPERATURE
        print "STEPS        :",self.STEPS
        print "NLSTATIC     :",self.nlstatic

#class for Abaqus *System header analysis
class ABQsystem:
    def __init__(self):
        self.a = [0.,0.,0.]
        self.b = [1.,0.,0.]
        self.c = [0.,1.,0.]
        self.v1 = [1.,0.,0.]
        self.v2 = [0.,1.,0.]
        self.v3 = [0.,0.,1.]
        self.v1v2valid = False
        self.v3valid = False
        self.idx_fm = '%7i'
        self.crd_fm = ',%12.4g'
        self.fm_setted = False

    def setSystem(self,line):
        tmp = line.replace(" ","").replace("\n","").replace("\r","").split(",")
        if not self.v1v2valid:
            self.v1v2valid = True
            self.a = [float(tmp[0]),float(tmp[1]),float(tmp[2])]
            if len(tmp) == 6:
                self.b = [float(tmp[3]),float(tmp[4]),float(tmp[5])]
                b = self.b
                a = self.a
                self.v1 = self.normalize([b[0]-a[0],b[1]-a[1],b[2]-a[2]])
                v1 = self.v1
                self.v2 = self.normalize([-v1[1],v1[0],v1[2]])
        elif not self.v3valid == True:
            self.v3valid = True
            self.c = [float(tmp[0]),float(tmp[1]),float(tmp[2])]
            c = self.c
            a = self.a
            self.v2 = self.normalize([c[0]-a[0],c[1]-a[1],c[2]-a[2]])
            x = self.v1
            y = self.v2
            self.v3 = self.normalize([x[1]*y[2]-x[2]*y[1],x[2]*y[0]-x[0]*y[2],x[0]*y[1]-x[1]*y[0]])

    def normalize(self,vec):
        sqr = 0.0
        for e in vec:
            sqr += e*e
        norm = math.sqrt(sqr)
        vec2 = []
        for e in vec:
            vec2.append(e/norm)
        return vec2

    def resetSystem(self):
        self.a = [0.,0.,0.]
        self.b = [1.,0.,0.]
        self.c = [0.,1.,0.]
        self.v1 = [1.,0.,0.]
        self.v2 = [0.,1.,0.]
        self.v3 = [0.,0.,1.]
        self.v1v2valid = False
        self.v3valid = False

    def setFormat(self,line):
        tmp = line.replace("\n","").replace("\r","").split(",")
        idx = str(len(tmp[0]))
        crd = str(len(tmp[1]))
        self.idx_fm = '%'+idx+'i'
        self.crd_fm = ',%'+crd+'.8g'
        self.fm_setted = True

    def getGrobalCoordinate(self,coord):
        x = self.a[0]+coord[0]*self.v1[0]+coord[1]*self.v2[0]+coord[2]*self.v3[0]
        y = self.a[1]+coord[0]*self.v1[1]+coord[1]*self.v2[1]+coord[2]*self.v3[1]
        z = self.a[2]+coord[0]*self.v1[2]+coord[1]*self.v2[2]+coord[2]*self.v3[2]
        return [x,y,z]

    def getGCline(self,dataline):
        if not self.fm_setted: self.setFormat(dataline)
        tmp = dataline.replace(" ","").replace("\n","").replace("\r","").split(",")
        nodenum = int(tmp[0])
        xyz = self.getGrobalCoordinate([float(tmp[1]),float(tmp[2]),float(tmp[3])])
        return self.idx_fm%nodenum+self.crd_fm%xyz[0]+self.crd_fm%xyz[1]+self.crd_fm%xyz[2]+"\n"

#class for *Element header analysis
class ElementSection:
    def __init__(self,TYPE,EGRP):
        self.type = TYPE
        self.egrp = EGRP
        self.dats = {}
        self.rangesetted = False
        self.range = []

    def datappend(self,dataline):
        tmp = dataline.replace(" ","").replace("\n","").replace("\r","").split(",")
        self.dats[int(tmp[0])] = tmp[1:]

    def setrange(self):
        if self.rangesetted: return
        nums = []
        for i in self.dats:
            nums.append(int(i))
        nums.sort()
        start = nums[0]
        end = nums[0]
        if len(nums) != 1 :
            for i in nums[1:]:
                if i-end > 1:
                    self.range.append([start,end])
                    start = i
                end = i
        self.range.append([start,end])
        self.rangesetted = True

    def isin(self,enum):
        isin = False
        for r in self.range:
            if r[0] <= enum and enum <= r[1] :
                isin = True
                break
        return isin


#convert header and options into [str,{},[]] format.
def header_analysis(myline):
    tmp = myline.split(",")
    header = tmp[0][1:].upper()
    options = {}
    options_noarg = []
    if len(tmp) > 1:
        for op in tmp[1:]:
            if op.find("=") >= 0:
                op_tmp = op.split("=")
                options[op_tmp[0].upper()] = op_tmp[1]
            else:
                options_noarg.append(op.upper())
    return [header,options,options_noarg]

### dataline reader ###
#set dataline read mode from header
def data_set(abq,mode,options,options_noarg):
    if mode == "NSET":
        isGenerate = "GENERATE" in options_noarg
        if options["NSET"] not in abq.NSETS.keys():
            abq.NSETS[options["NSET"]] = []
        abq.NSETS[options["NSET"]].append([isGenerate,[]])
    elif mode == "ELSET":
        isGenerate = "GENERATE" in options_noarg
        if options["ELSET"] not in abq.ELSETS.keys():
            abq.ELSETS[options["ELSET"]] = []
        abq.ELSETS[options["ELSET"]].append([isGenerate,[]])
    elif mode == "SOLIDSECTION":
        abq.SOLID_SECTIONS.append(options)
    elif mode == "SURFACE":
        abq.SURFACES[options["NAME"]] = [options["TYPE"],[]]
    elif mode == "MATERIAL":
        abq.curr_mat = options["NAME"]
        abq.MATERIALS[abq.curr_mat] = {}
    elif mode in MaterialProp:
        abq.MATERIALS[abq.curr_mat][mode] = {}
        cMAT = abq.MATERIALS[abq.curr_mat][mode]
        cMAT["DATALINES"] = []
        cMAT["DEPENDENCY"] = False
        cMAT["ORTHO"] = False
        if  mode == "ELASTIC":
            if "TYPE" in options.keys():
                if options["TYPE"] == "ENGINEERINGCONSTANTS":
                    cMAT["ORTHO"] = True
                elif options["TYPE"] == "ISOTROPIC":
                    pass
                else:
                    print "#Error: A2fconverter does support: *ELASTIC, TYPE="+options["TYPE"]
        elif  mode == "EXPANSION":
            if "TYPE" in options.keys():
                if options["TYPE"] == "ORTHO":
                    cMAT["ORTHO"] = True
            cMAT["REFTEMP"] = "0.0"
            if "ZERO" in options.keys():
                cMAT["REFTEMP"] = options["ZERO"]
        elif  mode == "HYPERELASTIC":
            abq.nlgeom = True
            cMAT["TYPE"] = "NONE"
            if "MOONEY-RIVLIN" in options_noarg:
                cMAT["TYPE"] = "MOONEY-RIVLIN"
            elif "OGDEN" in options_noarg:
                cMAT["TYPE"] = "OGDEN"
                cMAT["N"] = "1"
                if "N" in options.keys():
                    cMAT["N"] = options["N"]
        elif  mode == "VISCOELASTIC":
            if "TIME" in options.keys():
                if options["TIME"] != "PRONY":
                    print "#Error: A2fconverter does support: *VISCOELASTIC, TIME="+options["TIME"]
                    return False
        elif  mode == "TRS":
            if "DEFINITION" in options.keys():
                cMAT["DEFINITION"] = options["DEFINITION"]
        elif  mode == "PLASTIC":
            cMAT["YIELD"] = "MISES"
            cMAT["HARDEN"] = "ISOTROPIC"
            if "HARDENING" in options.keys():
                cMAT["HARDEN"] = "KINEMATIC"
        elif  mode == "DRUCKERPRAGER":
            cMAT["HARDEN"] = "ISOTROPIC"
        elif  mode == "CREEP":
            if "LAW" in options.keys():
                if options["LAW"] == "TIME":
                    return True
            print "FrontISTR supports only *CREEP, LAW=TIME"
            return False
    elif mode == "SURFACEINTERACTION":
        abq.curr_intprop = options["NAME"]
        abq.INT_PROPARTIES[abq.curr_intprop] = {}
    elif mode == "SURFACEBEHAVIOR":
        if 'NOSEPARATION' in options_noarg:
            abq.INT_PROPARTIES[abq.curr_intprop]['NOSEPARATION'] = True
    elif mode == "FRICTION":
        abq.INT_PROPARTIES[abq.curr_intprop][mode] = ""
    elif mode == "CONTACTPAIR":
        abq.curr_intprop = options["INTERACTION"]
        if "SMALLSLIDING" in options_noarg:
            abq.curr_slidtype ="SSLID"
        else:
            abq.curr_slidtype ="FSLID"
        if "ADJUST" in options.keys():
            abq.curr_adjust = options["ADJUST"].upper()
        else:
            abq.curr_adjust = "NO"
    elif mode == "TIE":
        abq.curr_slidtype = "TIED"
        if "POSITIONTOLERANCE" in options.keys():
            abq.curr_positiontol = options["POSITIONTOLERANCE"]
        else:
            abq.curr_positiontol = "0.0"
        if "ADJUST" in options.keys():
            abq.curr_adjust = options["ADJUST"].upper()
        else:
            abq.curr_adjust = "NO"
    elif mode == "AMPLITUDE":
        tmp = {}
        tmp["VALUE"] = "RELATIVE"
        if "VALUE" in options.keys():
            tmp["VALUE"] = options["VALUE"]
        if "TIME" in options.keys():
            if options["TIME"] == "TOTALTIME":
                print "#Error: A2fconverter does support: *AMPLITUDE, TIME=TOTAL TIME"
                return False
        tmp["DATALINES"] = []
        abq.AMP[options["NAME"]] = tmp
    elif mode == "BOUNDARY":
        abq.curr_boundary += 1
        abq.BOUNDARIES.append([])
        #for step control
        if "BOUNDARY" not in abq.STEPS[abq.curr_step].keys():
            abq.STEPS[abq.curr_step]["BOUNDARY"] = []
        tmpbnd = {}
        tmpbnd["ID"] = abq.curr_boundary
        tmpbnd["OP"] = ["MOD"]
        if "OP" in options.keys():
            tmpbnd["OP"] = options["OP"]
        abq.STEPS[abq.curr_step]["BOUNDARY"].append(tmpbnd)
    elif mode in LoadType:
        abq.curr_load += 1
        abq.LOADS.append({})
        abq.LOADS[abq.curr_load]["TYPE"] = mode
        if "AMPLITUDE" in options.keys():
            abq.LOADS[abq.curr_load]["AMPLITUDE"] = options["AMPLITUDE"]
        abq.LOADS[abq.curr_load]["DATALINES"] = []
        abq.LOADS[abq.curr_load]["MOMENT"] = False
        #for frequency analysis
        if "REAL" in options_noarg:
            abq.LOADS[abq.curr_load]["FLOADTYPE"] = "REAL"
        if "IMAGINARY" in options_noarg:
            abq.LOADS[abq.curr_load]["FLOADTYPE"] = "IMAGINARY"
        if "TYPE" in options.keys():
            abq.LOADS[abq.curr_load]["TYPE"] = options["TYPE"]
        if "DOF" in options.keys():
            abq.LOADS[abq.curr_load]["DOF"] = options["DOF"]
        abq.LOADS[abq.curr_load]["SCALE"] = "1.0"
        if "SCALE" in options.keys():
            abq.LOADS[abq.curr_load]["SCALE"] = options["SCALE"]
        #for step control
        if "LOAD" not in abq.STEPS[abq.curr_step].keys():
            abq.STEPS[abq.curr_step]["LOAD"] = []
        tmpld = {}
        tmpld["ID"] = abq.curr_load
        tmpld["OP"] = ["MOD"]
        if "OP" in options.keys():
            tmpld["OP"] = options["OP"]
        abq.STEPS[abq.curr_step]["LOAD"].append(tmpld)
    elif mode == "INITIALCONDITION" or mode == "INITIALCONDITIONS":
        abq.INITIAL_CONDITION["TYPE"] = options["TYPE"]
        abq.INITIAL_CONDITION["DATALINES"] = []
    elif mode == "STEP":
        abq.curr_step += 1
        abq.STEPS.append({})
        if "NLGEOM" in options.keys():
            if options["NLGEOM"] == "YES":
                abq.nlgeom = True
    elif mode == "STATIC":
        abq.STEPS[abq.curr_step]["STATIC"] = []
    elif mode == "VISCO":
        abq.nlgeom = True
        abq.STEPS[abq.curr_step]["VISCO"] = []
    elif mode == "FREQUENCY":
        abq.eigen = True
        abq.STEPS[abq.curr_step]["FREQ"] = []
    elif mode == "STEADYSTATEDYNAMICS":
        abq.freq_modal = True
        tmp = {}
        tmp["DAMPING"] = '1,1,0.0,0.0' #default damping parameter
        if "SUBSPACEPROJECTION" in options.keys()+options_noarg:
            print "#Error: A2fconverter does support: *STEADY STATE DYNAMICS, SUBSPACE PROJECTION"
            return False
        if "DIRECT" in options.keys():
            print "#Error: A2fconverter does support: *STEADY STATE DYNAMICS, DIRECT"
            return False
        tmp["SCALE"] = "LOGARITHMIC"
        if "FREQUENCYSCALE" in options.keys():
            tmp["SCALE"] = options["FREQUENCYSCALE"]
        tmp["INTERVAL"] = "EIGENFREQUENCY"
        if "INTERVAL" in options.keys():
            tmp["INTERVAL"] = options["INTERVAL"]
        abq.STEPS[abq.curr_step]["STEADYDYNA"] = tmp
    elif mode == "MODALDAMPING":
        if "MODAL" in options_noarg or "RAYLEIGH" not in options_noarg:
            print "#Error: A2fconverter does support: *MODAL DAMPING, MODAL"
            return False
        if "STRUCTURAL" in options_noarg:
            print "#Error: A2fconverter does support: *MODAL DAMPING, STRUCTURAL"
            return False
    elif mode == "SPRING":
        abq.curr_spring = options["ELSET"]
        abq.SPRINGS[abq.curr_spring] = []
    elif mode == "ORIENTATION":
        abq.curr_ori = options["NAME"]
        abq.ORIENTATION[abq.curr_ori] = {}
        for k in options.keys():
            if k != "NAME":
                abq.ORIENTATION[abq.curr_ori][k] = options[k]
        abq.ORIENTATION[abq.curr_ori]["DATALINES"] = []
    elif mode == "MPC":
        abq.MPC.append({})
    elif mode == "OUTPUT":
        abq.curr_fout += 1
        tmp = {}
        tmp["FREQUENCY"] = '1'
        if "FREQUENCY" in options.keys():
            tmp["FREQUENCY"] = options["FREQUENCY"]
        if "FIELD" in options_noarg:
            tmp["TYPE"] = "FIELD"
        elif "HISTORY" in options_noarg:
            tmp["TYPE"] = "HISTORY"
        elif "DIAGNOSTICS" in options_noarg:
            tmp["TYPE"] = "DIAGNOSTICS"
        if "VARIABLE" in options.keys():
            tmp["VARIABLE"] = options["VARIABLE"]
        for key in ['NODEOUTPUT',"CONTACTOUTPUT",'ELEMENTOUTPUT']:
            tmp[key] = []
        abq.FIELDOUTPUT.append(tmp)
    elif mode == "NODEOUTPUT" or mode == "CONTACTOUTPUT":
        tmp = {}
        if "NSET" in options.keys():
            tmp["NSET"] = options["NSET"]
        tmp["DATALINE"] = ""
        if "VARIABLE" in options.keys():
            tmp["VARIABLE"] = options["VARIABLE"]
        abq.FIELDOUTPUT[abq.curr_fout][mode].append(tmp)
    elif mode == "ELEMENTOUTPUT":
        tmp = {}
        if "ELSET" in options.keys():
            tmp["ELSET"] = options["ELSET"]
        tmp["DATALINE"] = ""
        if "VARIABLE" in options.keys():
            tmp["VARIABLE"] = options["VARIABLE"]
        abq.FIELDOUTPUT[abq.curr_fout][mode].append(tmp)
    else:
        print "*"+mode+" was skipped."
        mode == "DEFAULT"
    return True

#get dataline according to dataline read mode
def data_read(line,abq,mode,options,options_noarg):
    if mode == "NSET":
        abq.NSETS[options["NSET"]][-1][1].append(line)
    elif mode == "ELSET":
        abq.ELSETS[options["ELSET"]][-1][1].append(line)
    elif mode == "SOLIDSECTION":
        pass
    elif mode == "SURFACE":
        abq.SURFACES[options["NAME"]][1].append(line)
    elif mode == "MATERIAL":
        pass
    elif mode in MaterialProp:
        abq.MATERIALS[abq.curr_mat][mode]["DATALINES"].append(myread2(line))
    elif mode == "SURFACEINTERACTION":
        pass
    elif mode == "FRICTION":
        abq.INT_PROPARTIES[abq.curr_intprop][mode] = myread2(line)
    elif mode in "CONTACTPAIR":
        sm = myread(line).split(",")
        tmp = {}
        tmp["SLID_TYPE"] = abq.curr_slidtype
        tmp["INT_PROP"] = abq.curr_intprop
        tmp["SLAVE"] = sm[0]
        tmp["MASTER"] = sm[1]
        if abq.curr_adjust != "NO":
            tmp["ADJUST"] = abq.curr_adjust
        abq.CONTACT_PAIRS.append(tmp)
        abq.curr_adjust = "NO"
    elif mode in "TIE":
        sm = myread(line).split(",")
        tmp = {}
        tmp["SLID_TYPE"] = abq.curr_slidtype
        tmp["SLAVE"] = sm[0]
        tmp["MASTER"] = sm[1]
        tmp["POSITIONTOL"] = abq.curr_positiontol
        if abq.curr_adjust == "YES":
            tmp["ADJUST"] = "0.0"
        abq.CONTACT_PAIRS.append(tmp)
        abq.curr_adjust = "NO"
    elif mode == "AMPLITUDE":
        abq.AMP[options["NAME"]]["DATALINES"].append(line)
    elif mode == "BOUNDARY":
        abq.BOUNDARIES[abq.curr_boundary].append(line)
    elif mode in LoadType:
        abq.LOADS[abq.curr_load]["DATALINES"].append(line)
    elif mode == "INITIALCONDITION" or mode == "INITIALCONDITIONS":
        abq.INITIAL_CONDITION["DATALINES"].append(line)
    elif mode == "STEP":
        pass
    elif mode == "STATIC":
        abq.STEPS[abq.curr_step]["STATIC"].append(myread(line))
    elif mode == "VISCO":
        abq.STEPS[abq.curr_step]["VISCO"].append(myread(line))
    elif mode == "FREQUENCY":
        abq.STEPS[abq.curr_step]["FREQ"].append(myread(line))
    elif mode == "STEADYSTATEDYNAMICS":
        abq.STEPS[abq.curr_step]["STEADYDYNA"]["DATALINE"] = myread(line)
    elif mode == "MODALDAMPING":
        abq.STEPS[abq.curr_step]["STEADYDYNA"]["DAMP"] = myread(line)
    elif mode == "SPRING":
        abq.SPRINGS[abq.curr_spring].append(myread2(myread(line)))
    elif mode == "ORIENTATION":
        abq.ORIENTATION[abq.curr_ori]["DATALINES"].append(line)
    elif mode == "MPC":
        abq.MPC[-1]["DATALINE"] = line
    elif mode == "NODEOUTPUT" or mode == "CONTACTOUTPUT":
        abq.FIELDOUTPUT[abq.curr_fout][mode][-1]["DATALINE"] = line
    elif mode == "ELEMENTOUTPUT":
        abq.FIELDOUTPUT[abq.curr_fout][mode][-1]["DATALINE"] = line
    elif mode == "DEFAULT":
        pass
    return True

### write input ###
def getWriteList(fstrST,ELEMS):
    wlist = {}
    #1. Materials
    materials = set([])
    for s in fstrST.SECTIONS:
        materials.add(s["MATERIAL"])
    wlist["MATERIAL"] = list(materials)

    #2. Ngroups
    ngroups = set([])
    for bd in fstrST.BOUNDARIES:
        if len(bd)>0:
            for line in bd:
                ng = line.replace("\n","").replace(" ","").split(",")[0]
                if ng in fstrST.NGROUPS.keys():
                    ngroups.add(ng)
    for load in fstrST.LOADS:
        if load["TYPE"] == "CLOAD":
            if len(load["DATALINES"])>0:
                for line in load["DATALINES"]:
                    ng = line.replace("\n","").replace(" ","").split(",")[0]
                    ngroups.add(ng)
    for temp in fstrST.TEMPERATURE:
        if len(temp)>0:
            for line in temp:
                ng = line.replace("\n","").replace(" ","").split(",")[0]
                ngroups.add(ng)
    for cp in fstrST.CONTACT_PAIRS:
        ngroups.add(cp["SLAVE"])
    if not fstrST.INITIAL_CONDITION.keys() == []:
        if fstrST.INITIAL_CONDITION["TYPE"] == "TEMPERATURE":
            for line in fstrST.INITIAL_CONDITION["DATALINES"]:
                ng = line.replace("\n","").replace(" ","").split(",")[0]
                ngroups.add(ng)
    for nout in fstrST.OUTPUT['NODEOUT']:
        ngroups.add(nout['NGRP'])

    #add Ngroups for displacement output
    for key in fstrST.NGROUPS.keys():
        if key.find("DISPLACEMENT_OUTPUT") == 0:
            ngroups.add(key)
        if key.find("STRESS_OUTPUT") == 0:
            ngroups.add(key)
        if key.find("STRAIN_OUTPUT") == 0:
            ngroups.add(key)
    ngroups = ngroups.intersection(set(fstrST.NGROUPS.keys()))
    wlist["NGROUP"] = list(ngroups)

    #3. Egroups
    egroups = set([])
    degroups = set([])
    for sec in fstrST.SECTIONS:
        egroups.add(sec["ELSET"])
    for load in fstrST.LOADS:
        if "GRAV" in load.keys():
            if len(load["DATALINES"])>0:
                for line in load["DATALINES"]:
                    eg = line.replace("\n","").replace(" ","").split(",")[0]
                    if eg != "ALL":
                        egroups.add(eg)
    for eout in fstrST.OUTPUT['ELEMOUT']:
        egroups.add(eout['EGRP'])

    #remove Egroups already defined in !ELEMENT card.
    for elsec in ELEMS:
        if elsec.egrp is not "":
            degroups.add(elsec.egrp)
    egroups = egroups.difference(degroups)
    #add Egroups for strain/stress output
    for key in fstrST.EGROUPS.keys():
        if key.find("STRESS_OUTPUT") == 0:
            egroups.add(key)
        elif key.find("STRAIN_OUTPUT") == 0:
            egroups.add(key)
        elif key.find("PLSTRAIN_OUTPUT") == 0:
            egroups.add(key)
        elif key.find("NDSTRAIN_OUTPUT") == 0:
            egroups.add(key)
        elif key.find("NDSTRESS_OUTPUT") == 0:
            egroups.add(key)
    egroups = egroups.intersection(set(fstrST.EGROUPS.keys()))
    wlist["EGROUP"] = list(egroups)

    #4. Sgroups
    sgroups = set([])
    for load in fstrST.LOADS:
        if load["TYPE"] == "DLOAD" and "GRAV" not in load.keys():
            if len(load["DATALINES"])>0:
                for line in load["DATALINES"]:
                    sg = line.replace("\n","").replace(" ","").split(",")[0]
                    sgroups.add(sg)
    for cp in fstrST.CONTACT_PAIRS:
        sgroups.add(cp["MASTER"])
    wlist["SGROUP"] = list(sgroups)

    return wlist

def setVTK_OUTPUT(fstrST,wlist):

    if not fstrST.OUTPUT['VTKOUT']['DEFAULT']:
        return

    #1. creep/plastic strain
    for m in wlist["MATERIAL"]:
        if "CREEP" in fstrST.MATERIALS[m].keys():
            if 'CEEQ' not in fstrST.OUTPUT['VTKOUT']['VALUE']:
                fstrST.OUTPUT['VTKOUT']['VALUE'].append("CEEQ")
        if "PLASTIC" in fstrST.MATERIALS[m].keys():
            if 'PEEQ' not in fstrST.OUTPUT['VTKOUT']['VALUE']:
                fstrST.OUTPUT['VTKOUT']['VALUE'].append("PEEQ")

    #2. contact
    if len(fstrST.CONTACT_PAIRS)>0:
        if 'CREAC' not in fstrST.OUTPUT['VTKOUT']['VALUE']:
            fstrST.OUTPUT['VTKOUT']['VALUE'].append("CREAC")

def writeMshFile(fstrST,NODES,ELEMENTS,JOBNAME,wlist):
    f = open(JOBNAME+".msh","w")

    #1. !NODE
    NODES = NodeFormat(NODES)
    f.write("!NODE\n")
    f.writelines(NODES)

    #2. !ELEMENT
    idx = str(len(str(len(NODES)))+1)
    fm = ',%'+idx+'i'
    maxeid = 0
    for elsec in ELEMENTS:
        if elsec.type == '999':
            print 'ELEMENT TYPE = MASS is not converted.'
            continue
        line = "!ELEMENT, TYPE="+elsec.type
        if not elsec.egrp == "": line += ", EGRP="+nameMod(elsec.egrp)
        f.write(line+"\n")
        keys = elsec.dats.keys()
        keys.sort()
        if keys[-1] > maxeid:
            maxeid = keys[-1]
        datalines = []
        for e in keys:
            tmp = []
            for i in elsec.dats[e]:
                item = int(i)
                tmp.append(fm%item)
            datalines.append('%7i'%e+"".join(tmp)+"\n")
        f.writelines(datalines)
    if len(fstrST.ROTINFO)>0: #MPC CONSTRAINT
        f.write("!ELEMENT, TYPE=111, EGRP=ROTATIONLINK\n")
        for mpc in fstrST.ROTINFO:
            m = mpc["MASTER"]
            for slave in mpc["SLAVE"][:100]:
                maxeid += 1
                f.write('%7i'%maxeid+', '+m+', '+slave+'\n')

    #3. !NGROUP
    for ng in wlist["NGROUP"]:
        header = "!NGROUP, NGRP="+nameMod(ng)
        for sng in fstrST.NGROUPS[ng]:
            if sng[0]:
                f.write(header+", GENERATE\n")
            else:
                f.write(header+"\n")
            f.writelines(sng[1])

    #4. !EGROUP
    for eg in wlist["EGROUP"]:
        header = "!EGROUP, EGRP="+nameMod(eg)
        for seg in fstrST.EGROUPS[eg]:
            if seg[0]:
                f.write(header+", GENERATE\n")
            else:
                f.write(header+"\n")
            f.writelines(seg[1])

    #5. !SGROUP
    for sg in wlist["SGROUP"]:
        header = "!SGROUP, SGRP="+nameMod(sg)
        f.write(header+"\n")
        surfs = []
        tmp = []
        count = 0
        for sf in fstrST.SGROUPS[sg]:
            count += 1
            tmp.append(sf[0]+","+sf[1]+",")
            if count % 10 == 0:
                surfs.append("".join(tmp)+"\n")
                tmp = []
        if tmp != []:
            surfs.append("".join(tmp)+"\n")
        f.writelines(surfs)

    #6. !MATERIAL
    for mat in wlist["MATERIAL"]:
        N_ITEM = 1
        tmpMAT = fstrST.MATERIALS[mat]
        if "DENSITY" in tmpMAT.keys():
            N_ITEM = 2
        if "EXPANSION" in tmpMAT.keys():
            N_ITEM = 3
        f.write("".join(["!MATERIAL, NAME=",nameMod(mat),", ITEM=",str(N_ITEM),"\n"]))

        if "SPRING" not in tmpMAT.keys():
            f.write("!ITEM=1, SUBITEM=2\n")
        elif tmpMAT["SPRING"][1] == "":
            f.write("!ITEM=1, SUBITEM=1\n")
        else:
            f.write("!ITEM=1, SUBITEM=2\n")

        if "ELASTIC" in tmpMAT.keys():
            if tmpMAT["ELASTIC"]["DEPENDENCY"] or tmpMAT["ELASTIC"]["ORTHO"]:
                f.write('1000.,  0.3\n')
            else:
                f.writelines(tmpMAT["ELASTIC"]["DATALINES"])
        elif "SPRING" in tmpMAT.keys():
            f.write(tmpMAT["SPRING"][0])
        else:
            f.write('1000.,  0.3\n')
        if N_ITEM > 1:
            f.write("!ITEM=2, SUBITEM=1\n")
            if "DENSITY" in tmpMAT.keys():
                if tmpMAT["DENSITY"]["DEPENDENCY"]:
                    f.write('1.0\n')
                else:
                    f.writelines(tmpMAT["DENSITY"]["DATALINES"])
            else:
                f.write("1.0\n")
        if N_ITEM > 2:
            f.write("!ITEM=3, SUBITEM=1\n")
            if tmpMAT["EXPANSION"]["DEPENDENCY"] or tmpMAT["EXPANSION"]["ORTHO"]:
                f.write('1.0E-7\n')
            else:
                f.writelines(tmpMAT["EXPANSION"]["DATALINES"])
    if len(fstrST.ROTINFO)>0: #MPC CONSTRAINT
        f.write("!MATERIAL, NAME=ROTATION_MAT, ITEM=1\n")
        f.write("!ITEM=1, SUBITEM=1\n 0.0\n")

    #7. !CONTACT PAIRS
    ctid = 0
    for ct in fstrST.CONTACT_PAIRS:
        ctid += 1
        f.write("!CONTACT PAIR, NAME=CP"+str(ctid)+"\n")
        f.write(nameMod(ct["SLAVE"])+", "+nameMod(ct["MASTER"])+"\n")

    #8. !SECTION
    for sec in fstrST.SECTIONS:
        f.write("!SECTION, TYPE=SOLID, EGRP="+nameMod(sec["ELSET"])+", MATERIAL="+nameMod(sec["MATERIAL"])+"\n")
        f.write("1.0\n")
    if len(fstrST.ROTINFO)>0: #MPC CONSTRAINT
        f.write("!SECTION, TYPE=SOLID, EGRP=ROTATIONLINK, MATERIAL=ROTATION_MAT\n")
        f.write("1.0\n")

    #9. !INITIAL CONDITION
    if len(fstrST.INITIAL_CONDITION) > 0:
        f.write("!INITIAL CONDITION, TYPE="+fstrST.INITIAL_CONDITION["TYPE"]+"\n")
        for line in fstrST.INITIAL_CONDITION["DATALINES"]:
            f.write(convDatline(line))

    #10. !AMPLITUDE
    if len(fstrST.AMP) > 0:
        for ampk in fstrST.AMP.keys():
            name = nameMod(ampk)
            f.write("!AMPLITUDE, NAME="+name+", VALUE="+fstrST.AMP[ampk]["VALUE"]+"\n")
            for line in fstrST.AMP[ampk]["DATALINES"]:
                f.write(" "+line+"\n")

    f.close()

def writeCntFile(fstrST,JOBNAME,wlist,param):
    f = open(JOBNAME+".cnt","w")

    ###  SOLVER INFORMATION ###
    f.write("#"*35+"\n"+"#"*5+"   SOLVER INFORMATION    "+"#"*5+"\n"+"#"*35+"\n")
    #1. !VERSION
    f.write("!VERSION\n 3\n")
    #2. !SOLUTION
    if fstrST.nlstatic:
        f.write("!SOLUTION, TYPE=NLSTATIC\n")
    elif fstrST.eigen:
        if fstrST.freq_modal:
            f.write("!SOLUTION, TYPE=DYNAMIC\n")
        else:
            f.write("!SOLUTION, TYPE=EIGEN\n")
    else:
        if len(fstrST.CONTACT_PAIRS)>0:
            f.write("!SOLUTION, TYPE=NLSTATIC\n")
        else:
            f.write("!SOLUTION, TYPE=STATIC\n")
    #3. !CONTACT_ALGO
    if len(fstrST.CONTACT_PAIRS) > 0:
        f.write("!CONTACT_ALGO,TYPE=ALAGRANGE\n")
    #3.5. !AUTOCONTACT
    if param["AUTOCONTACT"]:
        f.write("!AUTO_CONTACT,TYPE="+param["ACTYPE"]+",LOG="+param["ACLOG"]+"\n")
        f.write(param["AC_CONTPARAM"]+"\n")
        f.write(param["AC_SEARCHPARAM"]+"\n")

    #4. !WRITE
    f.write("!WRITE,LOG\n")
    f.write("!NODE_OUTPUT\n")
    f.write("DISP,"+TF2OnOff(param["LOG_NODAL"]["DISP"])+"\n")
    f.write("REAC,"+TF2OnOff(param["LOG_NODAL"]["REAC"])+"\n")
    f.write("STRAIN,"+TF2OnOff(param["LOG_NODAL"]["STRAIN"])+"\n")
    f.write("STRESS,"+TF2OnOff(param["LOG_NODAL"]["STRESS"])+"\n")
    f.write("!ELEMENT_OUTPUT, POSITION=INTEG\n")
    f.write("STRAIN,"+TF2OnOff(param["LOG_ELEMENT"]["STRAIN"])+"\n")
    f.write("STRESS,"+TF2OnOff(param["LOG_ELEMENT"]["STRESS"])+"\n")
    f.write("PLSTRAIN,"+TF2OnOff(param["LOG_ELEMENT"]["PLSTRAIN"])+"\n")

    #5. !SOLVER
    if param["DIST"]:
        f.write("!SOLVER,METHOD=CG,PRECOND=3,NSET=0,ITERLOG=NO,TIMELOG=YES\n")
    else:
        f.write("!SOLVER,METHOD=DIRECT,PRECOND=1,NSET=0,ITERLOG=YES,TIMELOG=YES\n")
    f.write(" 500000, 2\n 1.00e-08, 1.00, 0.0\n 0.100, 0.100\n")

    #additional 1. !STABILIZE
    if param["IS_STAB_ACTIVE"]:
        f.write("!STABILIZE, STAB="+'%10.4E'%param["STAB"]+"\n")

    #additional 2. !CONTACT_SMOOTHING
    if param["CONTSTAB"]["LENGTH"] > 1.0e-10:
        STAB = 'STAB='+'%9.3E'%param["CONTSTAB"]["FACTOR"]
        DISTANCE = 'DISTANCE='+'%9.3E'%param["CONTSTAB"]["LENGTH"]
        f.write('!CONTACT_SMOOTHING, '+STAB+','+DISTANCE+"\n")

    #additional 3. !
    if param["OPCOLDAT"]:
        f.write("!OUTPUT_COLL_DATA\n")

    #additional 4. !VTK_OUTPUT_CONTROL
    f.write("!VTK_OUTPUT_CONTROL\n")
    for k in defaultVals:
        f.write(k+','+TF2OnOff(k in fstrST.OUTPUT['VTKOUT']['VALUE'])+'\n')
    for k in fstrST.OUTPUT['VTKOUT']['VALUE']:
        if k in defaultVals:
            continue
        f.write(k+',ON\n')

    #additional 5. !TXT_OUTPUT_NODE
    for nout in fstrST.OUTPUT['NODEOUT']:
        if nout['NGRP'] not in fstrST.NGROUPS.keys():
            continue
        fmt = 'SUMMARY'
        if int(nout['FREQUENCY']) > 1000:
            fmt = 'FULL'
        f.write('!TXT_OUTPUT_NODE, NGRP='+nameMod(nout['NGRP'])+', FORMAT='+fmt+', FREQ='+nout['FREQUENCY']+'\n')
        for val in nout['VALUE']:
            f.write(val+'\n')

    #additional 5. !TXT_OUTPUT_ELEM
    for eout in fstrST.OUTPUT['ELEMOUT']:
        if eout['EGRP'] not in fstrST.EGROUPS.keys():
            continue
        fmt = 'SUMMARY'
        if int(eout['FREQUENCY']) > 1000:
            fmt = 'FULL'
        f.write('!TXT_OUTPUT_ELEM, EGRP='+nameMod(eout['EGRP'])+', FORMAT='+fmt+', FREQ='+eout['FREQUENCY']+'\n')
        for val in eout['VALUE']:
            f.write(val+'\n')

    ###  BOUNDARY CONDITIONS ###
    f.write("#"*35+"\n"+"#"*5+"   BOUNDARY CONDITIONS   "+"#"*5+"\n"+"#"*35+"\n")
    #6. !BOUNDARY
    bdcount = 0
    for bd in fstrST.BOUNDARIES:
        bdcount += 1
        if len(bd)>0:
            f.write("!BOUNDARY, GRPID="+str(bdcount)+"\n")
            for line in bd:
                f.write(convDatline(line))

    #7. !CLOAD, !DLOAD, !TEMPERATURE, !ACCELERATION
    ldcount = 0
    for ld in fstrST.LOADS:
        ldcount += 1
        if len(ld["DATALINES"])>0:
            if ld["TYPE"] == "CLOAD":
                if "FLOADTYPE" in ld.keys():
                    f.write("!FLOAD, LOAD CASE="+RIkey[ld["FLOADTYPE"]])
                else:
                    f.write("!CLOAD, GRPID="+str(ldcount))
            elif ld["TYPE"] == "DLOAD":
                if "FLOADTYPE" in ld.keys():
                    f.write("!FLOAD, LOAD CASE="+RIkey[ld["FLOADTYPE"]])
                else:
                    f.write("!DLOAD, GRPID="+str(ldcount))
            elif ld["TYPE"] == "TEMPERATURE":
                f.write("!TEMPERATURE, GRPID="+str(ldcount))
            elif ld["TYPE"] == "ACCELERATION":
                f.write("!ACCELERATION")
            if "AMPLITUDE" in ld.keys():
                f.write(", AMP="+nameMod(ld["AMPLITUDE"]))
            f.write("\n")
            for line in ld["DATALINES"]:
                f.write(convDatline(line))

    ###  CONTACT DEFINITIONS ###
    #9. !CONTACT
    if len(fstrST.CONTACT_PAIRS) >0:
        f.write("#"*35+"\n"+"#"*5+"   CONTACT DEFINITIONS   "+"#"*5+"\n"+"#"*35+"\n")
        ctid = 0
        for ct in fstrST.CONTACT_PAIRS:
            ctid += 1
            line = "".join(["!CONTACT, INTERACTION=",ct["SLID_TYPE"],", "])
            if 'ADJUST' in ct.keys():
                line += "".join(['ADJUST=',ct["ADJUST"],', '])
            line += "".join( ["GRPID=",str(ctid),"\n"])
            f.write("".join(line))
            if ct["SLID_TYPE"] in ["SSLID","FSLID"]:
                PENALTY = param["CNTPENALTY"]
            elif ct["SLID_TYPE"] in ["TIED","SLIDONLY"]:
                PENALTY = param["TIEPENALTY"]
            f.write("".join(["CP",str(ctid),", ",ct["FRIC"],", "+PENALTY+"\n"]))

    if len(fstrST.ROTINFO)>0:
        for mpc in fstrST.ROTINFO:
            dof = mpc["AXIS"]
            if len(dof) == 1:
                dof = repr(int(dof)+3)
            f.write("!RIGIDLINK, DOF="+dof+'\n')
            m = mpc["MASTER"]
            for slave in mpc["SLAVE"]:
                f.write(' '+m+', '+slave+'\n')

    ###  MATERIAL  ###
    #10. !MATERIAL
    f.write("#"*35+"\n"+"#"*5+"         MATERIAL        "+"#"*5+"\n"+"#"*35+"\n")
    #10.1 !REFTEMP
    if len(fstrST.REFTEMP) > 0:
        f.write("!REFTEMP\n 0.0\n")
    #10.2 !ORIENTATION
    for kori in fstrST.ORIENTATION.keys():
        f.write("!ORIENTATION, NAME="+nameMod(kori)+"\n")
        f.write(fstrST.ORIENTATION[kori])
    #10.3 !SECTION
    count = 0
    for sec in fstrST.SECTIONS:
        count += 1
        if "ORIENTATION" in sec.keys():
            f.write("!SECTION, SECNUM="+repr(count)
                +", ORIENTATION="+nameMod(sec["ORIENTATION"])+"\n")
    #10.4 !MATERIAL
    for mat in wlist["MATERIAL"]:
        tmpMAT = fstrST.MATERIALS[mat]
        if "SPRING" in tmpMAT.keys():
            continue
        else:
            f.write("".join(["!MATERIAL, NAME=",nameMod(mat),"\n"]))

        #sort material keys
        if "TRS" in tmpMAT.keys():
            if "VISCOELASTIC" not in tmpMAT.keys():
                print "Error: *VISCOELASTIC card is necessary to use *TRS."
                return False
        tmpMATkeys = []
        for mat in MaterialProp: #sort Material keys in order of MaterialProp
            if mat in tmpMAT.keys():
                tmpMATkeys.append(mat)

        for kmat in tmpMATkeys:
            if kmat == "ELASTIC":
                card = "!ELASTIC"
                if param["HYPOELASTIC"]:
                    card += ", CAUCHY"
            elif kmat == "DENSITY":
                card = "!DENSITY"
            elif kmat == "EXPANSION":
                card = "!EXPANSION_COEFF"
                if tmpMAT[kmat]["REFTEMP"] != '0.0':
                    card += ", ZERO="+tmpMAT[kmat]["REFTEMP"]
            elif kmat == "PLASTIC":
                card = "!PLASTIC, YIELD="+tmpMAT[kmat]["YIELD"]
                card += ", HARDEN="+tmpMAT[kmat]["HARDEN"]
            elif kmat == "HYPERELASTIC":
                card = "!HYPERELASTIC, TYPE="+tmpMAT[kmat]["TYPE"]
            elif kmat == "VISCOELASTIC":
                card = "!VISCOELASTIC"
            elif kmat == "TRS":
                card = "!TRS"
                if "DEFINITION" in tmpMAT[kmat].keys():
                    card += ", DEFINITION="+tmpMAT[kmat]["DEFINITION"]
            elif kmat == "CREEP":
                card = "!CREEP"
            else:
                print "Error: unsupported material found ",kmat
                return False

            if tmpMAT[kmat]["ORTHO"]:
                card += ", TYPE=ORTHOTROPIC"
            if tmpMAT[kmat]["DEPENDENCY"]:
                card += ", DEPENDENCIES=1"

            f.write(card+'\n')
            for line in tmpMAT[kmat]["DATALINES"]:
                f.write(myread2(line))

    ###  STEP CONTROL  ###
    #11. !STEP
    f.write("#"*35+"\n"+"#"*5+"       STEP CONTROL      "+"#"*5+"\n"+"#"*35+"\n")
    stpcnt = -1
    n_freq = "1" #for !EIGENREAD
    for step in fstrST.STEPS:
        stpcnt += 1
        if "STATIC" in step.keys() or "VISCO" in step.keys():
            stpcnt = min(stpcnt,2)
            f.write("!STEP")
            if "VISCO" in step.keys():
                f.write(", TYPE=VISCO")
            if fstrST.nlstatic:
                f.write(", SUBSTEPS="+'%i'%param["STEPCTRL"][stpcnt]["N_SUBSTEPS"])
                if param["STEPCTRL"][stpcnt]["OUTFREQ"] > 0:
                    f.write(", OUTFREQ="+'%i'%param["STEPCTRL"][stpcnt]["OUTFREQ"])
                if param["STEPCTRL"][stpcnt]["SMOOTH"] > 0:
                    f.write(", SMOOTHING="+'%i'%param["STEPCTRL"][stpcnt]["SMOOTH"])
                f.write(", MAXITER="+'%i'%param["STEPCTRL"][stpcnt]["MAXITER"])
                if param["STEPCTRL"][stpcnt]["SMOOTH"] != 1.0E-6:
                    f.write(", CONVERG="+'%9.3E'%param["STEPCTRL"][stpcnt]["CONVERG"])
                f.write("\n")
            else:
                f.write(", SUBSTEPS=1\n")
            if "VISCO" in step.keys():
                f.write(step["VISCO"]+"\n")
            for bd in step["BOUNDARY"]:
                f.write("BOUNDARY,"+str(bd+1)+"\n")
            for ld in step["LOAD"]:
                f.write("LOAD,"+str(ld+1)+"\n")
            for tm in step["TEMPERATURE"]:
                f.write("LOAD,"+str(ldcount+tm+1)+"\n")
            if len(fstrST.CONTACT_PAIRS) >0:
                ctid = 0
                for ct in fstrST.CONTACT_PAIRS:
                    ctid += 1
                    f.write("CONTACT,"+str(ctid)+"\n")
        elif "FREQ" in step.keys():
            f.write("!EIGEN\n")
            f.write(step["FREQ"]["N_EIGEN"]+", 1.0E-8, "+step["FREQ"]["MAXITER"]+"\n")
            n_freq = step["FREQ"]["N_EIGEN"]
        elif "DYNAMIC" in step.keys():
            if step["DYNAMIC"]["TYPE"] == "FREQUENCY": #eigenread
                f.write("!EIGENREAD, TYPE=VTK\n")
                f.write(" dummy.vtk\n")
                f.write(" 1, "+n_freq+"\n")
            if fstrST.nlstatic:
                f.write("!DYNAMIC, TYPE=NONLINEAR\n")
            else:
                f.write("!DYNAMIC\n")
            if step["DYNAMIC"]["TYPE"] == "FREQUENCY":
                f.write(" 1, 2\n") #line 1
                f.write(" "+step["DYNAMIC"]["INT_LOW"]+", ") #line 2
                f.write(step["DYNAMIC"]["INT_HIGH"]+", ")
                f.write(step["DYNAMIC"]["N_POINT"]+", 1.0\n")
                f.write(" 0.0, 1.0\n") #line 3
                f.write(" 1, 1, ") #line 4
                f.write(step["DYNAMIC"]["ALPHA"]+", "+step["DYNAMIC"]["BETA"]+"\n")
                f.write(" 1, 1, 1\n") #line 5
                f.write(" 0, 0, 0, ") #line 6
                interval = 1
                if step["DYNAMIC"]["SCALE"] == "LOGARITHMIC":
                    interval += 1
                if step["DYNAMIC"]["INTERVAL"] == "EIGENFREQUENCY":
                    interval += 2
                f.write(repr(interval)+', 0, 0\n')
            else:
                print "#Error : unsupported type of dynamic analysis."
                return False


    ###  FOR VISUALIZER  ###
    #11. !VISUAL
    if param["OP_AVS"]:
        f.write("#"*35+"\n"+"#"*5+"      FOR VISUALIZER     "+"#"*5+"\n"+"#"*35+"\n")
        f.write("!WRITE,VISUAL\n")
        f.write("!VISUAL, method=PSR\n!surface_num=1\n!surface 1\n")
        f.write("!output_type = COMPLETE_REORDER_AVS\n")

    #12. !END
    f.write("!END\n")
    f.close()

    return True

def writeDatFile(JOBNAME,DIST,DOMAIN):
    f = open("hecmw_ctrl.dat","w")

    if DIST:
        f.write("!MESH, NAME=part_in,TYPE=HECMW-ENTIRE\n")
        f.write(JOBNAME+".msh\n")
        f.write("!MESH, NAME=part_out,TYPE=HECMW-DIST\n")
        f.write(JOBNAME+"_p\n")
        f.write("!MESH, NAME=fstrMSH,TYPE=HECMW-DIST\n")
        f.write(JOBNAME+"_p\n")
    else:
        f.write("!MESH, NAME=fstrMSH,TYPE=HECMW-ENTIRE\n")
        f.write(JOBNAME+".msh\n")
    f.write("!MESH, NAME=mesh,TYPE=HECMW-ENTIRE\n")
    f.write(JOBNAME+".msh\n")
    f.write("!CONTROL,NAME=fstrCNT\n")
    f.write(JOBNAME+".cnt\n")
    f.write("!RESTART,NAME=restart_out,IO=OUT\n")
    f.write(JOBNAME+".restart\n")
    f.write("!RESULT,NAME=fstrRES,IO=OUT\n")
    f.write(JOBNAME+".res\n")
    f.write("!RESULT,NAME=result,IO=IN\n")
    f.write(JOBNAME+".res\n")
    f.write("!RESULT,NAME=vis_out,IO=OUT\n")
    f.write(JOBNAME+".vis\n")

    f.close()

    if DIST:
        f = open("hecmw_part_ctrl.dat","w")
        f.write("!PARTITION,TYPE=NODE-BASED,METHOD=PMETIS,DOMAIN="+DOMAIN+"\n")
        f.close()
