#-------------------------------------------------------------------------------
# Name:        A2Fstruct
# Purpose:     "A2Fconverter" convert abaqus input file(.inp) into FISTR format
#              This program contains classes or sub-functions for A2Fconv_main.
#-------------------------------------------------------------------------------
#!/usr/bin/env python

import math
import re

## constant value ##
eltypeDic = {}
eltypeDic["C3D4"] = "341"
eltypeDic["C3D10"] = "342"
eltypeDic["C3D10M"] = "3423"
eltypeDic["C3D6"] = "351"
eltypeDic["C3D15"] = "352"
eltypeDic["C3D8"] = "361"
eltypeDic["C3D8I"] = "3612"
eltypeDic["C3D8H"] = "3614"
eltypeDic["C3D8R"] = "361"
eltypeDic["C3D20"] = "362"
eltypeDic["Spring1"] = "111"
eltypeDic["SpringA"] = "111"
eltypeDic["MASS"] = "999"

MaterialProp = ["DENSITY","ELASTIC","EXPANSION","HYPERELASTIC","PLASTIC",
                "VISCOELASTIC","TRS","DRUCKERPRAGER","CREEP"]
LoadType = ["CLOAD","DSLOAD","DLOAD","TEMPERATURE","BASEMOTION"]

valtypeDic = {}
valtypeDic["U"] = "DISP"
valtypeDic["V"] = "VELOCITY"
valtypeDic["A"] = "ACCELERATION"
valtypeDic["UR"] = "ROTATION"
valtypeDic["RF"] = "REAC"
valtypeDic["RM"] = "REACMOMENT"
valtypeDic["LE"] = "STRAIN"
valtypeDic["S"] = "STRESS"
valtypeDic["NT"] = "TEMPERATURE"
valtypeDic["PEEQ"] = "PEEQ"
valtypeDic["CEEQ"] = "CEEQ"
valtypeDic["CFORCE"] = "CONTREAC"
valtypeDic["CPRESS"] = "CONTPRESS"

defaultVals = ["DISP","REAC","STRAIN","STRESS"]

sf341 = [[0,1,2],[0,1,3],[1,2,3],[2,0,3]]
sf351 = [[0,1,2],[3,4,5],[0,1,4,3],[1,2,5,4],[2,0,3,5]]
sf361 = [[0,1,2,3],[4,5,6,7],[0,1,5,4],[1,2,6,5],[2,3,7,6],[3,0,4,7]]
sf342 = [[0,6,1,4,2,5],[0,6,1,8,3,7],[1,4,2,9,3,8],[2,5,0,9,3,7]]
sf352 = [[0,8,1,6,2,7],[3,11,4,9,5,10],[0,8,1,13,4,11,3],\
         [1,6,2,14,5,9,4],[2,7,0,12,3,10,5]]
sf362 = [[0,8,1,9,2,10,3,11],[4,12,5,13,6,14,7,15],[0,8,1,17,5,12,4,16],\
         [1,9,2,18,6,13,5,17],[2,10,3,19,7,14,6,18],[3,11,0,16,4,15,7,19]]

RIkey = {}
RIkey["REAL"] = "1"
RIkey["IMAGINARY"] = "2"

### utilitiy functions ###
#read line skipping its blanks and change line.
def myread(line):
    return line.replace(" ","").replace("\n","").replace("\r","")

def myread2(line):
    tmp = line.replace(",\n","\n").replace(",\r","\r")
    if tmp.find("e") >= 0 and tmp.find(".") <0:
        tmp = tmp.replace("e",".e")
    elif tmp.find("E") >= 0 and tmp.find(".") <0:
        tmp = tmp.replace("E",".E")
    return tmp

def mergeDatalineByNum(datalines,num):
    count = 0
    newdat = []
    tmpline = ""
    for line in datalines:
        count += 1
        if len(tmpline) == 0:
            tmpline = myread(line)
        else:
            tmpline += ","+myread(line)
        if count%num == 0:
            newdat.append(tmpline+'\n')
            tmpline = ""
    return newdat

def nameMod(name):
    name=name.replace("#","_shrp_")
    name=name.replace("/","_slsh_")
    name=name.replace("@","_at_")
    name=name.replace("+","_plus_")
    name=name.replace("$","_dl_")
    name=name.replace("%","_per_")
    name=name.replace("~","_tilda_")
    name=name.replace(",","_comma_")
    name=name.replace("(","_lb_")
    name=name.replace(")","_rb_")
    name=name.upper()
    if name == 'ALL':
        name = 'SETALL_GENERATED_BY_A2FCONV'
    return name

def convDatline(line):
    tmp = line.split(",")
    return ",".join([nameMod(tmp[0])]+tmp[1:])

def TF2OnOff(tf): #used for log_output
    if tf:
        return "ON"
    else:
        return "OFF"

### geometry functions ###
def convSurface(abqST):
    fstrSurfs = {}
    for k in abqST.SURFACES.keys():
        abqSurf = abqST.SURFACES[k]
        fstrSurf = ["",[]]
        if abqSurf[0] == "ELEMENT":
            fstrSurf[0] = "SGROUP"
        elif abqSurf[0] == "NODE":
            fstrSurf[0] = "NGROUP"
        #reading surface datalines
        data = []
        for line in abqSurf[1]:
            data.append(myread(line).split(","))
        if abqSurf[0] == "ELEMENT":
            for dat in data:
                egroup = abqST.ELSETS[dat[0]]
                snum = dat[1].replace("S","")
                for seg in egroup:
                    if seg[0]: #if generate
                        for dataline in seg[1]:
                            tmp = myread(dataline).split(",")
                            start = int(tmp[0])
                            end = int(tmp[1])+1
                            step = int(tmp[2])
                            for i in range(start,end,step):
                                fstrSurf[1].append([repr(i),snum])
                    else:
                        for dataline in seg[1]:
                            tmp = myread(dataline).split(",")
                            for item in tmp:
                                if item != "":
                                    fstrSurf[1].append([item,snum])
        elif abqSurf[0] == "NODE":
            for dat in data:
                fstrSurf[1] = abqST.NSETS[dat[0]]
        else: #unreachable
            print "unexpected error in abqST.convSurface"
            return
        fstrSurfs[k] = fstrSurf
    return fstrSurfs

def getNodeFromSurf(snum,etype):
    if etype == "341":
        return sf341[snum-1]
    elif etype == "351":
        return sf351[snum-1]
#    elif etype == "361" or etype == "3610" or etype == "3612":
    elif etype[0:3] == "361":
        return sf361[snum-1]
#    elif etype == "342" or etype == "3422" or etype == "3423":
    elif etype[0:3] == "342":
        return sf342[snum-1]
    elif etype == "352":
        return sf352[snum-1]
    elif etype == "362":
        return sf362[snum-1]

def getNodeFromNgroup(ngrpkey,NGROUPS):
    if ngrpkey not in NGROUPS:
        return []
    nodes = []
    for item in NGROUPS[ngrpkey]:
        if item[0]: #GENERATE
            tmp = myread(item[1][0]).split(",")
            start = int(tmp[0])
            end = int(tmp[1])
            interval = 1
            if len(tmp)>2:
                if tmp[2].isdigit():
                    interval = int(tmp[2])
            for i in range(start,end+1,interval):
                nodes.append(repr(i))
        else: #not GENERATE
            for line in item[1]:
                tmp = myread(line).split(",")
                for n in tmp:
                    if n != '':
                        nodes.append(n)
    return nodes

def makeNgroupFromSurf(surfdat,ELEMS):
    GENERATE = False
    DATS = []

    for e in ELEMS:
        e.setrange()
    for elsurf in surfdat:
        #get elem data
        enum = int(elsurf[0])
        snum = int(elsurf[1])
        edat = []
        etype = ""
        for e in ELEMS:
            if e.isin(enum):
                edat = e.dats[enum]
                etype = e.type
                break
        if edat == []:
            print "error in makeNgroupFromSurf"
            exit

        ids = getNodeFromSurf(snum,etype)
        for i in ids:
            DATS.append(int(edat[i]))

    DATS = set(DATS)
    DATS = list(DATS)
    DATS.sort()

    MAXDIGIT = int(math.floor(math.log10(DATS[-1])))+1
    s = '%'+str(MAXDIGIT+1)+'i,'

    #format
    count = 0
    tmp = []
    DATALINES = []
    for d in DATS:
        tmp.append(s%d)
        count += 1
        if count % 10 == 0:
            DATALINES.append("".join(tmp)+"\n")
            tmp = []
    if tmp != []:
        DATALINES.append("".join(tmp)+"\n")

    return [[GENERATE,DATALINES]]

def convContactPair(fstrST,abqST,ELEMS):
    fstrContactPairs = []
    for cp in abqST.CONTACT_PAIRS:
        fstrcp = {}
        fstrcp["SLID_TYPE"] = cp["SLID_TYPE"]
        fstrcp["MASTER"] = cp["MASTER"]
        if "ADJUST" in cp.keys():
            fstrcp["ADJUST"] = cp["ADJUST"]
        if "INT_PROP" in cp.keys():
            if "FRICTION" in abqST.INT_PROPARTIES[cp["INT_PROP"]].keys():
                fstrcp["FRIC"] = myread(abqST.INT_PROPARTIES[cp["INT_PROP"]]["FRICTION"])
            if 'NOSEPARATION' in abqST.INT_PROPARTIES[cp["INT_PROP"]].keys():
                fstrcp["SLID_TYPE"] = 'SLIDONLY'
                fstrcp["FRIC"] = "0.001"
        elif "POSITIONTOL" in cp.keys():
            fstrcp["FRIC"] = cp["POSITIONTOL"]
        else:
            fstrcp["FRIC"] = "0."

        #error check
        if cp["SLAVE"] not in abqST.SURFACES.keys():
            print 'Error in converting CONTACTPAIR.',
            print cp["SLAVE"],' is not found in surface group.'
            return 'ERROR'

        if abqST.SURFACES[cp["SLAVE"]][0] == "ELEMENT":
            surfData = fstrST.SGROUPS[cp["SLAVE"]]
            newNgroup = makeNgroupFromSurf(surfData,ELEMS)
            newNgName = cp["SLAVE"]+"_NGRP_"
            fstrST.NGROUPS[newNgName] = newNgroup
            fstrcp["SLAVE"] = newNgName
        else:
            fstrcp["SLAVE"] = cp["SLAVE"]
        fstrContactPairs.append(fstrcp)

    return fstrContactPairs

def convLoads(abqST):
    fstrLOADS = []
    for load in abqST.LOADS:
        if load["TYPE"] == "CLOAD":
            fstrLOADS.append(load)
        elif load["TYPE"] == "DSLOAD":
            fstrLOAD = {}
            fstrLOAD["TYPE"] = "DLOAD"
            fstrLOAD["DATALINES"] = []
            if "FLOADTYPE" in load.keys():
                fstrLOAD["FLOADTYPE"] = load["FLOADTYPE"]
            if len(load["DATALINES"])>0:
                for line in load["DATALINES"]:
                    tmp = myread(line).split(",")
                    tmp2 = tmp[0]+",S,"+tmp[2]+"\n"
                    fstrLOAD["DATALINES"].append(tmp2)
            if "AMPLITUDE" in load.keys():
                fstrLOAD["AMPLITUDE"] = load["AMPLITUDE"]
            fstrLOADS.append(fstrLOAD)
        elif load["TYPE"] == "DLOAD":
            fstrLOAD = {}
            fstrLOAD["TYPE"] = "DLOAD"
            if "FLOADTYPE" in load.keys():
                fstrLOADS["FLOADTYPE"] = load["FLOADTYPE"]
            if len(load["DATALINES"])>0:
                datline = load["DATALINES"][0]
                tmp = myread(datline).split(",")
                if tmp[0] == "":
                    datline = "ALL"+datline
                fstrLOAD["DATALINES"] = [datline]
            else:
                fstrLOAD["DATALINES"] = []
            if "AMPLITUDE" in load.keys():
                fstrLOAD["AMPLITUDE"] = load["AMPLITUDE"]
            fstrLOAD["GRAV"] = True
            fstrLOADS.append(fstrLOAD)
        elif load["TYPE"] == "TEMPERATURE":
            fstrLOADS.append(load)
        elif load["TYPE"] == "ACCELERATION":
            tmpload = load
            tmpload["DATALINES"].append(" ALL, "+load["DOF"]+", "+load["DOF"]+", "+load["SCALE"]+"\n")
            fstrLOADS.append(tmpload)
    return fstrLOADS

def convBoundaryCondition(abqST):
    boundary = []
    for bd in abqST.BOUNDARIES:
        tmpbd = []
        if len(bd)>0:
            for line in bd:
                tmp = line.replace("\n","").replace("\r","").replace(" ","").split(",")
                t1 = tmp[0]
                t3 = ""
                new_line = " "+t1+","
                if len(tmp) > 2: t3 = tmp[2]
                if tmp[1] in ["1","2","3"]:
                    if tmp[2] in ["1","2","3"]:
                        new_line += tmp[1]+","+tmp[2]
                    else:
                        new_line += tmp[1]+",3"
                    if len(tmp) > 3:
                        new_line += ","+tmp[3]
                    tmpbd.append(new_line+"\n")
                elif tmp[1] in ["PINNED","ENCASTRE"]:
                    tmpbd.append(" "+t1+", 1, 3\n")
                elif tmp[1] == 'XSYMM':
                    tmpbd.append(" "+t1+", 1, 1\n")
                elif tmp[1] == 'YSYMM':
                    tmpbd.append(" "+t1+", 2, 2\n")
                elif tmp[1] == 'ZSYMM':
                    tmpbd.append(" "+t1+", 3, 3\n")
                elif tmp[1] == 'XASYMM':
                    tmpbd.append(" "+t1+", 2, 3\n")
                elif tmp[1] == 'YASYMM':
                    tmpbd.append(" "+t1+", 1, 1\n")
                    tmpbd.append(" "+t1+", 3, 3\n")
                elif tmp[1] == 'ZASYMM':
                    tmpbd.append(" "+t1+", 1, 2\n")
                else:
                    continue
        boundary.append(tmpbd)
    return boundary

def convDepPlastic(datalines):
    DIG = '%14.7E'
    DIG_OUT ='%12.6G'
    if len(datalines) < 2:
        return datalines

    #data read
    dat = []
    plst = set([])
    temp = set([])
    for line in datalines:
        tmp = myread(line).split(",")
        for i in [0,1,2]:
            tmp[i] = DIG%float(tmp[i])
        plst.add(tmp[1])
        temp.add(tmp[2])
        dat.append(tmp)

    plst = list(plst)
    plst.sort(cmp=lambda x,y: cmp(float(x), float(y)))
    temp = list(temp)
    temp.sort(cmp=lambda x,y: cmp(float(x), float(y)))

    #init table
    table = {}
    for ktmp in temp:
        table[ktmp] = {}
        for kpl in plst:
            table[ktmp][kpl] = 'None'
    for item in dat:
        table[item[2]][item[1]] = item[0]

    #interporate table
    for ktmp in temp:
        if table[ktmp][plst[0]] == 'None':
            print 'initial plastic strain must be zero at temperature',ktmp
            return 'ERROR'
        for i in range(1,len(plst)):
            if table[ktmp][plst[i]] != 'None':
                continue
            current_strain = float(plst[i])
            prev_strain = float(plst[i-1])
            prev_mises = float(table[ktmp][plst[i-1]])
            #seek next
            next_strain = current_strain
            next_mises = 'None'
            for kpl in plst[i+1:]:
                if table[ktmp][kpl] != 'None':
                    next_strain = float(kpl)
                    next_mises = float(table[ktmp][kpl])
                    break
            if next_mises == 'None':
                table[ktmp][plst[i]] = table[ktmp][plst[i-1]]
                continue
            #interporate
            ratio1 = (next_strain-current_strain)/(next_strain-prev_strain)
            ratio2 = (current_strain-prev_strain)/(next_strain-prev_strain)
            value = ratio1*prev_mises+ratio2*next_mises
            table[ktmp][plst[i]] = DIG%value

    #format data
    data = []
    for ktmp in temp:
        temperature = DIG_OUT%float(ktmp)
        for kpl in plst:
            plstrain = DIG_OUT%float(kpl)
            mises = DIG_OUT%float(table[ktmp][kpl])
            line = ', '.join([mises, plstrain, temperature])
            data.append(line+'\n')

    return data

def convMaterials(abqST):
    fstrMAT = {}
    for kmat in abqST.MATERIALS.keys():
        mat = abqST.MATERIALS[kmat]
        for k in mat.keys():
            #modify datline
            if len(mat[k]["DATALINES"])>0:
                tmp0 = myread(mat[k]["DATALINES"][0]).split(",")
                if k == "ELASTIC":
                    newdataline = []
                    if mat[k]["ORTHO"]:
                        tmp1 = myread(mat[k]["DATALINES"][1]).split(",")
                        if len(tmp1) > 1:
                            mat[k]["DEPENDENCY"] = True
                        mat[k]["DATALINES"] = mergeDatalineByNum(mat[k]["DATALINES"],2)
                    else:
                        if len(tmp0) > 2:
                            mat[k]["DEPENDENCY"] = True
                elif k == "EXPANSION":
                    if mat[k]["ORTHO"]:
                        if len(tmp0) > 3:
                            mat[k]["DEPENDENCY"] = True
                    else:
                        if len(tmp0) > 1:
                            mat[k]["DEPENDENCY"] = True
                elif k == "DENSITY":
                    if len(tmp0) > 1:
                        mat[k]["DEPENDENCY"] = True
                elif k == "HYPERELASTIC":
                    deg = ""
                    if 'N' in mat[k].keys():
                        deg = mat[k]["N"]
                    if deg in ["3","4"]:
                        mat[k]["DATALINES"] = mergeDatalineByNum(mat[k]["DATALINES"],2)
                elif k == "VISCOELASTIC":
                    mat[k]["DATALINES"] = convViscoDatLine(mat[k]["DATALINES"])
                elif k == "PLASTIC":
                    if mat[k]["HARDEN"]=="ISOTROPIC":
                        if len(mat[k]["DATALINES"]) > 1:
                            mat[k]["HARDEN"] = "MULTILINEAR"
                        elif len(mat[k]["DATALINES"]) == 1:
                            mat[k]["HARDEN"] = "BILINEAR"
                        if len(tmp0) > 2:
                            mat[k]["DEPENDENCY"] = True
                            mat[k]["DATALINES"] = convDepPlastic(mat[k]["DATALINES"])
                            if mat[k]["DATALINES"] == 'ERROR':
                                print 'Error in converting material:',kmat,k
                                return 'ERROR'
                    elif mat[k]["HARDEN"]=="KINEMATIC":
                        if len(tmp0) > 2:
                            mat[k]["DEPENDENCY"] = True
                    else:
                        if len(tmp0) > 3:
                            mat[k]["DEPENDENCY"] = True
                elif k == "CREEP":
                    if len(tmp0) > 3:
                        mat[k]["DEPENDENCY"] = True

            if k == "HYPERELASTIC":
                if mat[k]["TYPE"] == "OGDEN":
                    mat[k]["TYPE"] += "_A"+mat[k]["N"]

        fstrMAT[kmat] = mat

    return fstrMAT

def convViscoDatLine(datalines):
    newdat = []
    for line in datalines:
        tmp = line.split(",")
        del tmp[1]
        newdat.append(",".join(tmp))
    return newdat

def convStep(abqST):
    n_step = len(abqST.STEPS)-1
    fstrSTEP = []
    for i in range(n_step):
        tmp = {}
        tmp["BOUNDARY"] = []
        tmp["LOAD"] = []
        tmp["TEMPERATURE"] = []
        fstrSTEP.append(tmp)

    for cond in ["BOUNDARY","LOAD"]:
        if cond in abqST.STEPS[0]:
            for fstrstp in fstrSTEP:
                for abqstp in abqST.STEPS[0][cond]:
                    fstrstp[cond].append(abqstp["ID"])
        if len(abqST.STEPS) > 0:
            c_stp = 0
            for abqstp in abqST.STEPS[1:]:
                c_stp += 1
                if cond in abqstp.keys():
                    OPisNEW = False
                    for bd in abqstp[cond]:
                        if bd["OP"] == "NEW":
                            OPisNEW = True
                    if OPisNEW:
                        for i in range(c_stp-1,n_step):
                            fstrSTEP[i][cond] = []
                    for bd in abqstp[cond]:
                        if cond == "BOUNDARY" and len(abqST.BOUNDARIES[bd["ID"]])>0:
                            for i in range(c_stp-1,n_step):
                                fstrSTEP[i][cond].append(bd["ID"])
                        if cond == "LOAD" and len(abqST.LOADS[bd["ID"]]["DATALINES"])>0:
                            fstrSTEP[c_stp-1][cond].append(bd["ID"])

    c_stp = -1
    for abqstp in abqST.STEPS[1:]:
        c_stp += 1
        if "VISCO" in abqstp.keys():
            fstrSTEP[c_stp]["VISCO"] = ','.join(abqstp["VISCO"][0].split(",")[:2])
        elif "STATIC" in abqstp.keys():
            fstrSTEP[c_stp]["STATIC"] = abqstp["STATIC"]
        elif "FREQ" in abqstp.keys():
            tmp = abqstp["FREQ"][0].split(",")
            fstrSTEP[c_stp]["FREQ"] = {}
            fstrSTEP[c_stp]["FREQ"]["N_EIGEN"] = tmp[0]
            fstrSTEP[c_stp]["FREQ"]["MAXITER"] = repr(5*int(tmp[0]))
        elif "STEADYDYNA" in abqstp.keys():
            tmp = {}
            tmp["TYPE"] = "FREQUENCY"
            tmp["SCALE"] = abqstp["STEADYDYNA"]["SCALE"]
            tmp["INTERVAL"] = abqstp["STEADYDYNA"]["INTERVAL"]
            tmpline = myread2(abqstp["STEADYDYNA"]["DATALINE"]).split(",")
            tmp["INT_LOW"] = tmpline[0]
            tmp["INT_HIGH"] = tmpline[1]
            tmp["N_POINT"] = tmpline[2]
            tmpline = myread2(abqstp["STEADYDYNA"]["DAMP"]).split(",")
            tmp["ALPHA"] = tmpline[2]
            tmp["BETA"] = tmpline[3]
            fstrSTEP[c_stp]["DYNAMIC"] = tmp
    return fstrSTEP

def convSprings(abqST,fstrST):
    if len(abqST.SPRINGS) == 0:
        return
    SPNum = 0
    for key in abqST.SPRINGS.keys():
        SPNum += 1
        SPMatName = "SPRING-"+repr(SPNum)

        #add section
        tmpSection = {}
        tmpSection["ELSET"] = key
        tmpSection["MATERIAL"] = SPMatName
        fstrST.SECTIONS.append(tmpSection)

        #add material
        spstiff = abqST.SPRINGS[key][1]
        spdof = abqST.SPRINGS[key][0]
        datline = spstiff
        if spdof is not "":
            datline += ", "+spdof
        datline += "\n"
        fstrST.MATERIALS[SPMatName] = {}
        fstrST.MATERIALS[SPMatName]["SPRING"] = [datline,spdof]

def convOrientation(abqST):
    fstrORI = {}
    for kori in abqST.ORIENTATION.keys():
        ori = abqST.ORIENTATION[kori]
        if "DEFINITION" in ori.keys():
            if ori["DEFINITION"] != "COORDINATES":
                print "Error: orientation must be defined by coordinates."
                return 'ERROR'
        tmp = ori["DATALINES"][0]
        if len(tmp.split(",")) == 6:
            tmp = myread(tmp)+",0.0,0.0,0.0"

        fstrORI[kori] = tmp+"\n"
    return fstrORI

def convROTinfo(abqST,fstrST):
    fstrROTINFO = []

    if len(abqST.MPC) == 0:
        return fstrROTINFO

    #boundary data
    bnd_dat = []
    for bnd in abqST.BOUNDARIES:
        tmpbnd = {}
        for dof in range(1,7):
            tmpbnd[repr(dof)] = 'None'
        for line in bnd:
            tmp = myread(line).split(",")
            tmpbnd["NAME"] = tmp[0]
            if tmp[1].isdigit():
                start = int(tmp[1])
                end = int(tmp[2])
                val = '%12.5E'%0.0
                if len(tmp)>3:
                    val = '%12.5E'%float(tmp[3])
                for dof in range(start,end+1):
                    tmpbnd[repr(dof)] = val
            else:
                val = '%12.5E'%0.0
                if tmp[1] == "PINNED":
                    for dof in [1,2,3]:
                        tmpbnd[repr(dof)] = val
                elif tmp[1] == "ENCASTRE":
                    for dof in [1,2,3,4,5,6]:
                        tmpbnd[repr(dof)] = val
                else:
                    print "Converting boundary condition \""+tmp[1] +"\" is not supported."
                    return 'ERROR'
        tmpbnd["DAT"] = getNodeFromNgroup(tmpbnd["NAME"],fstrST.NGROUPS)
        bnd_dat.append(tmpbnd)

    #MPC data
    for mpc in abqST.MPC:
        tmp = myread(mpc["DATALINE"]).split(',')
        if tmp[0] == 'BEAM':
            pass
        elif tmp[0] == 'TIE':
            pass
        else:
            print "Error(MPC): FrontISTR does not support MPC type:",tmp[0]
            return 'ERROR'
        master_grp = tmp[2]
        slave_grp = tmp[1]

        #get master node
        if master_grp.isdigit():
            master = master_grp
        else:
            master0 = getNodeFromNgroup(master_grp,fstrST.NGROUPS)
            if len(master0)==0:
                print "Error(MPC): master node not found in NGROUP:",master_grp
                return 'ERROR'
            elif len(master0)>1:
                print "Error(MPC): more than one master nodes found in :",master_grp
                return 'ERROR'
            master = master0[0]

        #get slave node
        slave = getNodeFromNgroup(slave_grp,fstrST.NGROUPS)
        if len(slave) == 0:
            print "Error(MPC):slave node group not found",slave_grp
            return 'ERROR'

        if tmp[0] == 'BEAM':
            #cload
            mload = []
            load_id = -1
            cdof = {}
            for dof in range(1,7):
                cdof[repr(dof)] = 'None'
            for load in fstrST.LOADS:
                load_id += 1
                if load["TYPE"] != 'CLOAD':
                    continue
                for line in load["DATALINES"]:
                    tmpline = myread(line).split(",")
                    ngrp = tmpline[0]
                    ngrpdat = getNodeFromNgroup(ngrp,fstrST.NGROUPS)
                    if ngrp == master_grp or ngrpdat == [master]:
                        if load_id not in mload:
                            mload.append(load_id)
                        if abs(float(tmpline[2])) > 1.0e-10:
                            cdof[tmpline[1]] = tmpline[2]
                        continue

            #boundary
            mbound = []
            bnd_id = -1
            bdof = {}
            for dof in range(1,7):
                bdof[repr(dof)] = 'None'
            for bnd in bnd_dat:
                bnd_id += 1
                if bnd["DAT"] == [master]:
                    mbound.append(bnd_id)
                    for dof in range(1,7):
                        if bnd[repr(dof)] != 'None':
                            bdof[repr(dof)] = bnd[repr(dof)]
                    continue

            # get rotation axis
            axis = 'None'
            cload = 'None'
            moment = 'None'
            for dof in ['1','2','3']:
                if cdof[dof] != 'None':
                    axis = dof
                    cload = cdof[dof]
            for dof in ['4','5','6']:
                if cdof[dof] != 'None':
                    tmp_axis = repr(int(dof)-3)
                    if axis != 'None' and axis != tmp_axis:
                        print "Error(MPC): dof error in cload: master id=",master
                        return 'ERROR'
                    axis = tmp_axis
                    moment = cdof[dof]
            for dof in ['1','2','3','4','5','6']:
                if bdof[dof] == 'None' or bdof[dof] != '%12.5E'%0.0:
                    tmp_axis = dof
                    if int(dof) > 3:
                        tmp_axis = repr(int(dof)-3)
                    if axis != 'None' and axis != tmp_axis:
                        print "Error(MPC): dof error in boundary: master id=",master
                        return 'ERROR'
                    axis = tmp_axis

            # modify cload and boundary
            for i in mload:
                tmpline = []
                for line in fstrST.LOADS[i]["DATALINES"]:
                    tmp = myread(line).split(",")
                    if tmp[1] == axis and cload != 'None':
                        tmpline.append(tmp[0]+', 1, '+cload+'\n')
                    if int(tmp[1]) == int(axis)+3 and moment != 'None':
                        tmpline.append(tmp[0]+', 2, '+moment+'\n')
                fstrST.LOADS[i]["DATALINES"] = tmpline

            mbndline = []
            if bdof[axis] != 'None':
                val = ''
                if bdof[axis] != '%12.5E'%0.0:
                    val = ',%9.6F'%float(bdof[axis])
                mbndline.append(master+', 1, 1 '+val+'\n')
            if bdof[repr(int(axis)+3)] != 'None':
                val = ''
                if bdof[repr(int(axis)+3)] != '%12.5E'%0.0:
                    val = ',%9.6F'%float(bdof[repr(int(axis)+3)])
                mbndline.append(master+', 2, 2 '+val+'\n')
            mbndline.append(master+', 3, 3\n')
            fstrST.BOUNDARIES[mbound[0]] = mbndline
            for i in mbound[1:]:
                fstrST.BOUNDARIES[i] = [master+', 3, 3\n']

        elif tmp[0] == 'TIE':
            axis = '123'

        # make rigidlink data
        tmpmpc = {}
        tmpmpc["MASTER"] = master
        tmpmpc["SLAVE"] = slave
        tmpmpc["AXIS"] = axis
        fstrROTINFO.append(tmpmpc)

    return fstrROTINFO

def convFieldOutput(abqST):
    fstrOUT = {}
    fstrOUT["VTKOUT"] = {}
    fstrOUT["VTKOUT"]["DEFAULT"] = False
    fstrOUT["VTKOUT"]["VALUE"] = []
    tmp_NODEOUT = []
    tmp_ELEMOUT = []
    fstrOUT["NODEOUT"] = []
    fstrOUT["ELEMOUT"] = []

    for output in abqST.FIELDOUTPUT:
        if output["TYPE"] != "FIELD":
            continue
        if "VARIABLE" in output.keys():
            if output["VARIABLE"] == "PRESELECT":
                fstrOUT["VTKOUT"]["DEFAULT"] = True
                for item in defaultVals:
                    fstrOUT["VTKOUT"]["VALUE"].append(item)
                continue
        for k in ['NODEOUTPUT',"CONTACTOUTPUT",'ELEMENTOUTPUT']:
            for dat in output[k]:
                items0 = myread(dat['DATALINE']).split(',')
                items = []
                for item in items0:
                    if item in valtypeDic.keys():
                        items.append(item)
                if len(items) == 0:
                    continue
                tmp = {}
                tmp['VALUE'] = []
                tmp['FREQUENCY'] = '1'
                if 'FREQUENCY' in output.keys():
                    tmp['FREQUENCY'] = output['FREQUENCY']
                if 'NSET' in dat:
                    tmp['NGRP'] = dat['NSET']
                    for item in items:
                        tmp['VALUE'].append(valtypeDic[item])
                    tmp_NODEOUT.append(tmp)
                elif 'ELSET' in dat:
                    tmp['EGRP'] = dat['ELSET']
                    for item in items:
                        tmp['VALUE'].append(valtypeDic[item])
                    tmp_ELEMOUT.append(tmp)
                else:
                    for item in items:
                        fstrOUT["VTKOUT"]['VALUE'].append(valtypeDic[item])
    fstrOUT["VTKOUT"]["VALUE"] = list(set(fstrOUT["VTKOUT"]["VALUE"]))

    if len(tmp_NODEOUT) > 0:
        fstrOUT["NODEOUT"] = [tmp_NODEOUT[0]]
        for item0 in tmp_NODEOUT[1:]:
            flag = False
            for item in fstrOUT["NODEOUT"]:
                if item0['NGRP'] == item['NGRP']:
                    if item0['VALUE'] == item['VALUE']:
                        flag = True
            if not flag:
                fstrOUT["NODEOUT"].append(item0)
    if len(tmp_ELEMOUT) > 0:
        fstrOUT["ELEMOUT"] = [tmp_ELEMOUT[0]]
        for item0 in tmp_ELEMOUT[1:]:
            flag = False
            for item in fstrOUT["ELEMOUT"]:
                if item0['EGRP'] == item['EGRP']:
                    if item0['VALUE'] == item['VALUE']:
                        flag = True
            if not flag:
                fstrOUT["ELEMOUT"].append(item0)
    return fstrOUT

def convAmplitude(abqST):
    fstrAMP = {}
    for ampk in abqST.AMP.keys():
        amp = abqST.AMP[ampk]
        tmp = {}
        tmp["VALUE"] = amp["VALUE"]
        dat = []
        for line in amp["DATALINES"]:
            tmpline = myread(line).split(",")
            dat += tmpline
        fstrdat = []
        while len(dat) > 0:
            time = dat.pop(0)
            value = dat.pop(0)
            fstrdat.append(value)
            fstrdat.append(time)
        tmp["DATALINES"] = []
        for i in range(0,len(fstrdat)/8):
            tmpline = ", ".join(fstrdat[8*i:8*i+8])
            tmp["DATALINES"].append(tmpline)
        if len(fstrdat)%8 != 0:
            tmpline = ", ".join(fstrdat[-(len(fstrdat)%8):])
            tmp["DATALINES"].append(tmpline)
        fstrAMP[ampk] = tmp
    return fstrAMP

def setRefTemp(abqST):
    reftemps = set([])
    for kmat in abqST.MATERIALS.keys():
        mat = abqST.MATERIALS[kmat]
        if "EXPANSION" in mat.keys():
            if mat["EXPANSION"]["DEPENDENCY"]:
                reftemps.add(mat["EXPANSION"]["REFTEMP"])

    reftemps = list(reftemps)
    if len(reftemps) == 0:
        return ''
    elif len(reftemps) == 1:
        return reftemps[0]
    else:
        print "Error: All reference temperature values must be same."
        print "RefTemps: ",reftemps
        return 'ERROR'

#modify irregular input
def A2Fpost(fstrST,ELEMS):
    #egroup of section is empty
    EGROUPS = fstrST.EGROUPS.keys()
    for elemsection in ELEMS:
        if elemsection.egrp != "":
            EGROUPS.append(elemsection.egrp)
    for sec in fstrST.SECTIONS:
        egrp = sec["ELSET"]
        if egrp not in EGROUPS:
            fstrST.SECTIONS.remove(sec)
    for sec in fstrST.SECTIONS:
        egrp = sec["ELSET"]
        if egrp not in EGROUPS:
            print sec
    return True

#modify numbering of tetra-2nd element
def tetra2mod(myline):
    nodes = myline.split(",")
    tmp = nodes[5]
    nodes[5] = nodes[6]
    nodes[6] = nodes[7]
    nodes[7] = tmp
    newline = nodes[0]
    for n in nodes[1:]:
        newline += ","+n
    return newline + "\n"

#modify numbering of spring1 element
def spring1mod(myline):
    nodes = myline.split(",")
    return nodes[0]+","+nodes[1]+","+nodes[1]+"\n"

def NodeFormat(NODES):
    N_NODES = []
    ptn = re.compile("[\+|\-|\s][1-9]+[e|E][\+|\-][0-9]*")
    for line in NODES:
        m = re.search(ptn,line)
        if m is not None:
            tmp = myread(line).split(",")
            length = len(tmp[1])
            fmt = '  %'+str(length-2)+'.5e'
            idx = tmp[0]+','
            crd1 = fmt%float(tmp[1])+','
            crd2 = fmt%float(tmp[2])+','
            crd3 = fmt%float(tmp[3])+'\n'
            n_line = "".join([idx,crd1,crd2,crd3])
            N_NODES.append(n_line)
        else:
            N_NODES.append(line)
    return N_NODES


