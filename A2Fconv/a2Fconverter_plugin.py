# -*- coding: utf-8 -*-
from abaqusGui import *
from abaqusConstants import ALL
import osutils, os


###########################################################################
# Class definition
###########################################################################

class a2Fconverter_plugin(AFXForm):

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def __init__(self, owner):

        # Construct the base class.
        #
        AFXForm.__init__(self, owner)
        self.radioButtonGroups = {}

        self.cmd = AFXGuiCommand(mode=self, method='main',
            objectName='A2F', registerQuery=False)
        pickedDefault = ''
        self.INPUT_FILEKw = AFXStringKeyword(self.cmd, 'INPUT_FILE', True, '')
        self.CONVALLKw = AFXBoolKeyword(self.cmd, 'CONVALL', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.STEPSKw = AFXStringKeyword(self.cmd, 'STEPS', True, '5')
        self.IS_STAB_ACTIVEKw = AFXBoolKeyword(self.cmd, 'IS_STAB_ACTIVE', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.STABKw = AFXStringKeyword(self.cmd, 'STAB', True, '1.0e-6')
        self.LDISPKw = AFXBoolKeyword(self.cmd, 'LDISP', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.LREACKw = AFXBoolKeyword(self.cmd, 'LREAC', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.LNSTRAINKw = AFXBoolKeyword(self.cmd, 'LNSTRAIN', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.LNSTRESSKw = AFXBoolKeyword(self.cmd, 'LNSTRESS', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.LESTRAINKw = AFXBoolKeyword(self.cmd, 'LESTRAIN', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.LESTRESSKw = AFXBoolKeyword(self.cmd, 'LESTRESS', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.LPLSTKw = AFXBoolKeyword(self.cmd, 'LPLST', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.OPFREQKw = AFXStringKeyword(self.cmd, 'OPFREQ', True, '1')
        self.AL1Kw = AFXStringKeyword(self.cmd, 'AL1', True, '5')
        self.AL2Kw = AFXStringKeyword(self.cmd, 'AL2', True, '1')
        self.AL3Kw = AFXStringKeyword(self.cmd, 'AL3', True, '100')
        self.PCNTKw = AFXStringKeyword(self.cmd, 'PCNT', True, '1.0e+3')
        self.PTIEKw = AFXStringKeyword(self.cmd, 'PTIE', True, '1.0e+3')
        self.CONTSTABLENGTHKw = AFXStringKeyword(self.cmd, 'CONTSTABLENGTH', True, '0.0')
        self.CONTSTABFACTORKw = AFXStringKeyword(self.cmd, 'CONTSTABFACTOR', True, '0.001')
        self.OPAVSKw = AFXBoolKeyword(self.cmd, 'OPAVS', AFXBoolKeyword.TRUE_FALSE, True, False)
        self.STEPCTRLKw = AFXTableKeyword(self.cmd, 'STEPCTRL', True)
        self.STEPCTRLKw.setColumnType(0, AFXTABLE_TYPE_INT)
        self.STEPCTRLKw.setColumnType(1, AFXTABLE_TYPE_INT)
        self.STEPCTRLKw.setColumnType(2, AFXTABLE_TYPE_INT)
        self.STEPCTRLKw.setColumnType(3, AFXTABLE_TYPE_INT)
        self.STEPCTRLKw.setColumnType(4, AFXTABLE_TYPE_FLOAT)
        self.OPCOLDATKw = AFXBoolKeyword(self.cmd, 'OPCOLDAT', AFXBoolKeyword.TRUE_FALSE, True, True)

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def getFirstDialog(self):

        import a2FconverterDB
        return a2FconverterDB.a2FconverterDB(self)

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def doCustomChecks(self):

        # Try to set the appropriate radio button on. If the user did
        # not specify any buttons to be on, do nothing.
        #
        for kw1,kw2,d in self.radioButtonGroups.values():
            try:
                value = d[ kw1.getValue() ]
                kw2.setValue(value)
            except:
                pass
        return True

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def okToCancel(self):

        # No need to close the dialog when a file operation (such
        # as New or Open) or model change is executed.
        #
        return False

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Register the plug-in
#
thisPath = os.path.abspath(__file__)
thisDir = os.path.dirname(thisPath)

toolset = getAFXApp().getAFXMainWindow().getPluginToolset()
toolset.registerGuiMenuButton(
    buttonText='A2Fconverter',
    object=a2Fconverter_plugin(toolset),
    messageId=AFXMode.ID_ACTIVATE,
    icon=None,
    kernelInitString='import A2F',
    applicableModules=ALL,
    version='N/A',
    author='N/A',
    description='N/A',
    helpUrl='N/A'
)
