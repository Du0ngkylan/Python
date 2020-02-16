#!/usr/bin/env python

from A2F import *
import sys
import os
import shutil

def A2Flaunch(INPUT_FILE="",STEPS="5",IS_STAB_ACTIVE=False,STAB="1.0e-6",AL1="5",AL2="1",AL3="100", \
            LDISP=False,LREAC=True,LNSTRAIN=False,LNSTRESS=False, \
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
        if len(item) > 5:
            param["STEPCTRL"][cnt]["VISCOINC"] = item[5]
        cnt += 1

    if(INPUT_FILE==''):
        print 'A2fconverter error: no input file.'
        return
    if INPUT_FILE.find("/"):
        FILE = INPUT_FILE.split("/")[-1]
        PATH = INPUT_FILE[:INPUT_FILE.find(FILE)]
    else:
        FILE = INPUT_FILE
        PATH = '.'
    if FILE not in os.listdir(os.curdir):
        print 'Cannot find ' + FILE
        return
    DIRNAME = FILE[:-4]
    os.chdir(os.curdir)
    if DIRNAME not in os.listdir(os.curdir):
        os.mkdir(DIRNAME)

    os.chdir('./'+DIRNAME)
    main0(INPUT_FILE='../'+INPUT_FILE,param=param)
    os.chdir('../')

def main(INPUTFILE):
    f = open("A2Fsetup.ini")
    dat = f.readlines()
    f.close()

    IS_STAB_ACTIVE=False
    STAB="0.0"
    PCNT="1.0E+3"
    PTIE="1.0E+3"
    STEPCTRL = [{},{},{},{}]
    CONTSTABLENGTH='0.0'
    CONTSTABFACTOR='0.001'
    OPCOLDAT=True
    OPAVS=False
    HYPOELASTIC=False
    AUTOCONTACT= False
    ACTYPE='SSLID'
    ACLOG='SUMMARY'
    ACFRIC='0.2'
    ACPEN='1.0e+3'
    ACDEG_LOCAL='60.0'
    ACDEG_GLOBAL='30.0'
    ACDIST='10.0'
    ACMARGIN='-1.0e-6'
    DIST=False
    DOMAIN='4'

    for line in dat:
        if line[0] == "#":
            continue
        tmp = line.replace("\n","").replace("\r","").replace(" ","")
        if len(tmp) == 0:
            continue
        tmp0 = tmp.split("=")
        header0 = tmp0[0].split("_")
        header = header0[0].upper()
        num = ""
        if len(header0) > 1:
            num = int(header0[1])
        val = tmp0[1]

        if header == "STAB":
            STAB = val
            if abs(float(STAB)) > 1.0e-10:
                IS_STAB_ACTIVE=True
        elif header == "NSTEP":
            STEPCTRL[num-1][header] = int(val)
        elif header == "SMOOTHING":
            STEPCTRL[num-1][header] = int(val)
        elif header == "NEWTON":
            STEPCTRL[num-1][header] = int(val)
        elif header == "OUTFREQ":
            STEPCTRL[num-1][header] = int(val)
        elif header == "CONVERG":
            STEPCTRL[num-1][header] = float(val)
        elif header == "VISCOINC":
            STEPCTRL[num-1][header] = int(val)
        elif header == "CPENALTY":
            PCNT = val
        elif header == "TPENALTY":
            PTIE = val
        elif header == "SMOOTHING-DIST":
            CONTSTABLENGTH = val
        elif header == "SMOOTHING-FACT":
            CONTSTABFACTOR = val
        elif header == "OUTPUT-COL-DAT":
            OPCOLDAT = (val == "YES")
        elif header == "HYPOELASTIC":
            HYPOELASTIC = (val == "YES")
        elif header == "AUTOCONTACT":
            AUTOCONTACT = (val == "YES")
        elif header == "ACTYPE":
            ACTYPE = val
        elif header == "ACLOG":
            ACLOG = val
        elif header == "ACFRIC":
            ACFRIC = val
        elif header == "ACPEN":
            ACPEN = val
        elif header == "ACDEGLOCAL":
            ACDEG_LOCAL = val
        elif header == "ACDEGGLOBAL":
            ACDEG_GLOBAL = val
        elif header == "ACDIST":
            ACDIST = val
        elif header == "ACMARGIN":
            ACMARGIN = val
        elif header == "DISTRIBUTE":
            DIST = (val == "YES")
        elif header == "DOMAINNUM":
            DOMAIN = val
        elif header == "OUTPUTAVS":
            OPAVS = (val == "YES")

    STEPCTRL2 = []
    for stp in STEPCTRL:
        if len(stp)==0:
            continue
        tmp = []
        for k in ["NSTEP","SMOOTHING","NEWTON","OUTFREQ","CONVERG","VISCOINC"]:
            if k in stp.keys():
                tmp.append(stp[k])
        STEPCTRL2.append(tmp)

    A2Flaunch(INPUTFILE,"5",IS_STAB_ACTIVE,STAB,"5","1","100", \
              False,False,False,False, \
              False,False,False,"1",PCNT,PTIE, \
              OPAVS,STEPCTRL2,False,CONTSTABLENGTH,CONTSTABFACTOR, \
              OPCOLDAT,HYPOELASTIC,AUTOCONTACT,ACTYPE,ACLOG, \
              ACFRIC,ACPEN,ACDEG_LOCAL,ACDEG_GLOBAL,ACDIST,ACMARGIN,DIST,DOMAIN)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'Usage: # python A2Flaunch.py ABAQUS_INPFILE.inp'
        quit()

    main(sys.argv[1])
