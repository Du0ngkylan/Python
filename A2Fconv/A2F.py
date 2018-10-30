# -*- coding: utf-8 -*-
#-------------------------------------------------------------------------------
# Name:        A2F
# Purpose:     This program convert abaqus input file(*.inp) into
#              FrontISTR format.
#-------------------------------------------------------------------------------
#!/usr/bin/env python

from A2Flib import *
from A2Fstruct import *
import sys
import os
import shutil

def ReadAbaqusInputFile(abqST,NODE,ELEMS,INPUT_FILE):
    input_file = open(INPUT_FILE,'r')

    #flags
    mode = "DEFAULT" #controls reading mode. usually equals header name.

    system = ABQsystem()
    abq = abqST

    print "start reading Abaqus input file..."
    #read lines
    line = input_file.readline()
    while line:
        if line[:2] == "**": #pass comment line
            pass
        elif line[0] == "*": #read header line
            myline = myread(line)
            if myline[-1] == ",":
                myline += input_file.readline()
                myline = myread(myline)
            ha = header_analysis(myline)
            mode = ha[0]
            options = ha[1]
            options_noarg = ha[2]
            if mode == "NODE":
                print "reading *Node data"
                pass
            elif mode == "SYSTEM":
                system.resetSystem()
            elif mode == "ELEMENT":
                print "reading *Element data, etype="+options["TYPE"]
                tmp_elem_section = ElementSection(eltypeDic[options["TYPE"]],"")
                if "ELSET" in options.keys():
                    tmp_elem_section.egrp = options["ELSET"]
                ELEMS.append(tmp_elem_section)
            elif mode == "PART":
                print "Error: A2Fconverter does not support Part-Assembly formatted input file."
                return False
            else:
                if not data_set(abq,mode,options,options_noarg):
                    return False
        else: #read data lines
            if mode == "NODE":
                if system.v1v2valid:
                    line = system.getGCline(line)
                NODE.append(line)
            elif mode == "SYSTEM":
                system.setSystem(line)
            elif mode == "ELEMENT":
                tmpline = myread(line)
                if tmpline[-1] == ",":
                    tmpline += myread(input_file.readline())
                if options["TYPE"][:5] in ["C3D10","C3D10M"]:
                    modline = tetra2mod(tmpline)
                    ELEMS[-1].datappend(modline)
                elif options["TYPE"][:7].upper() == "SPRING1":
                    modline = spring1mod(tmpline)
                    ELEMS[-1].datappend(modline)
                else:
                    modline = tmpline+'\n'
                    ELEMS[-1].datappend(modline)
            else:
                if not data_read(line,abq,mode,options,options_noarg):
                    return False

        #read next line
        line = input_file.readline()

    #close files
    input_file.close()
    return True

def A2Fconv(fstrST,abqST,ELEMS):
    print "converting NGROUP,EGROUP,SECTIONS..."
    fstrST.NGROUPS = abqST.NSETS
    fstrST.EGROUPS = abqST.ELSETS
    fstrST.SECTIONS = abqST.SOLID_SECTIONS

    #convert SGROUPS
    print "converting SGROUP..."
    tmpSGROUP = convSurface(abqST)
    for k in tmpSGROUP.keys():
        if tmpSGROUP[k][0] == "SGROUP":
            fstrST.SGROUPS[k] = tmpSGROUP[k][1]
        elif tmpSGROUP[k][0] == "NGROUP":
            fstrST.NGROUPS[k] = tmpSGROUP[k][1]
            del tmpSGROUP[k]

    print "converting MATERIALS..."
    fstrST.MATERIALS = convMaterials(abqST)
    if fstrST.MATERIALS == 'ERROR': return False

    print "converting CONTACT_PAIRS..."
    fstrST.CONTACT_PAIRS = convContactPair(fstrST,abqST,ELEMS)
    if fstrST.CONTACT_PAIRS == 'ERROR': return False

    print "converting OTHERS..."
    fstrST.AMP = convAmplitude(abqST)
    if fstrST.AMP == 'ERROR': return False
    fstrST.BOUNDARIES = convBoundaryCondition(abqST)
    if fstrST.BOUNDARIES == 'ERROR': return False
    fstrST.LOADS = convLoads(abqST)
    fstrST.ROTINFO = convROTinfo(abqST,fstrST)
    if fstrST.ROTINFO == 'ERROR': return False
    fstrST.INITIAL_CONDITION = abqST.INITIAL_CONDITION
    fstrST.STEPS = convStep(abqST)
    fstrST.nlstatic = abqST.nlgeom
    fstrST.eigen = abqST.eigen
    fstrST.freq_modal = abqST.freq_modal

#    fstrST.REFTEMP = setRefTemp(abqST)
#    if fstrST.REFTEMP == 'ERROR': return False

    fstrST.ORIENTATION = convOrientation(abqST)
    if fstrST.ORIENTATION == 'ERROR': return False
    convSprings(abqST,fstrST)
    fstrST.OUTPUT = convFieldOutput(abqST)

    return A2Fpost(fstrST,ELEMS)

def WriteFistrInputFiles(fstrST,NODES,ELEMENTS,JOBNAME,param):
    #1.get writelist(delete unneccessary data)
    wlist = getWriteList(fstrST,ELEMENTS)
    setVTK_OUTPUT(fstrST,wlist)
    #2.write *.cnt file
    print "writing .cnt file..."
    ctrl = writeCntFile(fstrST,JOBNAME,wlist,param)
    if not ctrl: return False
    #3.write *.msh file
    print "writing .msh file..."
    writeMshFile(fstrST,NODES,ELEMENTS,JOBNAME,wlist)
    #4.write hecmw_ctrl.dat
    print "writing hecmw_ctrl.dat file..."
    writeDatFile(JOBNAME,param["DIST"],param["DOMAIN"])
    return True

### main function ###
def main0(INPUT_FILE,param):
    #structs
    NODES = []
    ELEMENTS = []
    abq = abqST()
    fstr = fstrST()
    JOBNAME = INPUT_FILE.split("/")[-1][:-4]

    ### STEP1: READ ABAQUS INPUT FILE ###
    print "### STEP1(READ ABAQUS INPUT FILE) start. ###"
    ctrl = ReadAbaqusInputFile(abq,NODES,ELEMENTS,INPUT_FILE)
    if not ctrl: return False
    print "### STEP1(READ ABAQUS INPUT FILE) end.   ###"

    ### STEP2: CONVERT DATA STRUCTURE INTO FRONTISTR FORMAT ###
    print "### STEP2(CONVERT DATA STRUCTURE INTO FRONTISTR FORMAT) start. ###"
    ctrl = A2Fconv(fstr,abq,ELEMENTS)
    if not ctrl: return False
    print "### STEP2(CONVERT DATA STRUCTURE INTO FRONTISTR FORMAT) end.   ###"

    ### STEP3: OUTPUT FRONTISTER INPUT FILES ###
    print "### STEP3(WRITE FRONTISTER INPUT FILES) start. ###"
    ctrl = WriteFistrInputFiles(fstr,NODES,ELEMENTS,JOBNAME,param)
    if not ctrl: return False
    print "### STEP3(WRITE FRONTISTER INPUT FILES) end. ###"

    return True

def SetOutDirectory(curdir):
    filelist = os.listdir(curdir)
    inpfiles = []
    for fn in filelist:
        if fn[-4:] == '.inp':
            inpfiles.append(fn)

    for inp in inpfiles:
        jobname = inp[:-4]
        DIRNAME = './'+jobname
        if jobname not in filelist:
            os.mkdir(DIRNAME)
#        shutil.copy(src=inp, dst=DIRNAME)
    return inpfiles

def ConvertAll(inpfiles,param):
    for inp in inpfiles:
        jobname = inp[:-4]
        DIRNAME = './'+jobname
        os.chdir(DIRNAME)
        inpfile = '../'+inp
        main0(INPUT_FILE=inpfile,param=param)
        os.chdir('../')

def MakeSubmitScript(inpfiles,settings):
    f=open("submitall.sh","w")
    for inp in inpfiles:
        jobname = inp[:-4]
        f.write("cd "+jobname+"\n")
        f.write("qsub "+settings["QSUB_SCRIPT"]+"\n")
        f.write("cd ../\n\n")
    f.close()

def main(INPUT_FILE="",STEPS="5",IS_STAB_ACTIVE=False,STAB="1.0e-6",AL1="5",AL2="1",AL3="100", \
            LDISP=False,LREAC=False,LNSTRAIN=False,LNSTRESS=False, \
            LESTRAIN=False,LESTRESS=False,LPLST=False,OPFREQ="1",PCNT="1.0E+3",PTIE="1.0E+3", \
            OPAVS=False,STEPCTRL=[],CONVALL=False,CONTSTABLENGTH='0.0',CONTSTABFACTOR='0.001', \
            OPCOLDAT=False,HYPOELASTIC=False,AUTOCONTACT=False,ACTYPE='SSLID',ACLOG='SUMMARY', \
            ACFRIC='0.2',ACPEN='1.0e+3',ACDEG_LOCAL='60.0',ACDEG_GLOBAL='30.0',ACDIST='10.0',ACMARGIN='-1.0e-6', \
            DIST=False,DOMAIN='1'):

    #parameters
    param = {}
    param["N_SUBSTEPS"] = int(STEPS) #integer
    param["IS_STAB_ACTIVE"] = IS_STAB_ACTIVE
    param["STAB"] = float(STAB) #float
    param["AL"] = [int(AL1),int(AL2),int(AL3)]
    param["CONTSTAB"] = {}
    param["CONTSTAB"]["LENGTH"] = float(CONTSTABLENGTH)
    param["CONTSTAB"]["FACTOR"] = float(CONTSTABFACTOR)
    param["LOG_NODAL"] = {}
    param["LOG_NODAL"]["DISP"] = LDISP
    param["LOG_NODAL"]["REAC"] = LREAC
    param["LOG_NODAL"]["STRAIN"] = LNSTRAIN
    param["LOG_NODAL"]["STRESS"] = LNSTRESS
    param["LOG_ELEMENT"] = {}
    param["LOG_ELEMENT"]["STRAIN"] = LESTRAIN
    param["LOG_ELEMENT"]["STRESS"] = LESTRESS
    param["LOG_ELEMENT"]["PLSTRAIN"] = LPLST
    param["OUTPUT_FREQUENCY"] = int(OPFREQ)
    param["CNTPENALTY"] = PCNT
    param["TIEPENALTY"] = PTIE
    param["OP_AVS"] = OPAVS
    param["STEPCTRL"] = []
    param["OPCOLDAT"] = OPCOLDAT
    param["HYPOELASTIC"] = HYPOELASTIC
    param["AUTOCONTACT"] = AUTOCONTACT
    param["ACTYPE"] = ACTYPE
    param["ACLOG"] = ACLOG
    param["AC_CONTPARAM"] = ','.join([ACFRIC,ACPEN])
    param["AC_SEARCHPARAM"] = ','.join([ACDEG_LOCAL,ACDEG_GLOBAL,ACDIST,ACMARGIN])
    param["DIST"] = DIST
    param["DOMAIN"] = DOMAIN

    for i in range(3):
        param["STEPCTRL"].append({})
        param["STEPCTRL"][i]["N_SUBSTEPS"] = 1
        param["STEPCTRL"][i]["SMOOTH"] = 0
        param["STEPCTRL"][i]["MAXITER"] = 50
        param["STEPCTRL"][i]["OUTFREQ"] = 1
        param["STEPCTRL"][i]["CONVERG"] = 1.0E-6
        param["STEPCTRL"][i]["VISCOINC"] = 1

    cnt = 0
    for item in STEPCTRL:
        param["STEPCTRL"][cnt]["N_SUBSTEPS"] = max(item[0],1)
        param["STEPCTRL"][cnt]["SMOOTH"] = item[1]
        if item[2] != 0:
            param["STEPCTRL"][cnt]["MAXITER"] = item[2]
        param["STEPCTRL"][cnt]["OUTFREQ"] = max(item[3],1)
        if item[4] != 0:
            param["STEPCTRL"][cnt]["CONVERG"] = item[4]
        cnt += 1

    if not CONVALL:
        if(INPUT_FILE==''):
            print 'A2fconverter error: no input file.'
            return
        if INPUT_FILE.find("/") > -1:
            FILE = INPUT_FILE.split("/")[-1]
            PATH = INPUT_FILE[:INPUT_FILE.find(FILE)]
        else:
            FILE = INPUT_FILE
            PATH = './'
            INPUT_FILE = '../'+FILE
        if FILE not in os.listdir(PATH):
            print 'Cannot find ' + FILE
            return
        DIRNAME = FILE[:-4]
        os.chdir(PATH)
        if DIRNAME not in os.listdir('./'):
            os.mkdir(DIRNAME)

        os.chdir('./'+DIRNAME)
        main0(INPUT_FILE=INPUT_FILE,param=param)
        os.chdir('../')
    else:
        inpfiles=SetOutDirectory('./')
        ConvertAll(inpfiles,param)

        #job submit script
        settings = {}
        settings["QSUB_SCRIPT"] = '/share/tool/x86_64/FISTR_v4/FISTR_bld6.sh'

        MakeSubmitScript(inpfiles,settings)

if __name__ == '__main__':
    main("sample.inp")
