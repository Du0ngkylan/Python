# -*- coding: utf-8 -*-
from abaqusConstants import *
from abaqusGui import *
from kernelAccess import mdb, session
import os

thisPath = os.path.abspath(__file__)
thisDir = os.path.dirname(thisPath)


###########################################################################
# Class definition
###########################################################################

class a2FconverterDB(AFXDataDialog):

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def __init__(self, form):

        # Construct the base class.
        #

        AFXDataDialog.__init__(self, form, 'A2Fconv',
            self.OK|self.CANCEL, DIALOG_ACTIONS_SEPARATOR)


        okBtn = self.getActionButton(self.ID_CLICKED_OK)
        okBtn.setText('OK')

        l = FXLabel(p=self, text='変換するAbaqus入力ファイルを指定してください（拡張子.inp）：', opts=JUSTIFY_LEFT)
        l.setFont( getAFXFont(FONT_BOLD) )
        fileHandler = a2FconverterDBFileHandler(form, 'INPUT_FILE', '*.inp')
        fileTextHf = FXHorizontalFrame(p=self, opts=0, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0, hs=DEFAULT_SPACING, vs=DEFAULT_SPACING)
        # Note: Set the selector to indicate that this widget should not be
        #       colored differently from its parent when the 'Color layout managers'
        #       button is checked in the RSG Dialog Builder dialog.
        fileTextHf.setSelector(99)
        AFXTextField(p=fileTextHf, ncols=50, labelText='入力ファイル：', tgt=form.INPUT_FILEKw, sel=0,
            opts=AFXTEXTFIELD_STRING|LAYOUT_CENTER_Y)
        icon = afxCreatePNGIcon(thisDir+r'\fileselect.png')
        FXButton(p=fileTextHf, text='	Select File\nFrom Dialog', ic=icon, tgt=fileHandler, sel=AFXMode.ID_ACTIVATE,
            opts=BUTTON_NORMAL|LAYOUT_CENTER_Y, x=0, y=0, w=0, h=0, pl=1, pr=1, pt=1, pb=1)
        FXCheckButton(p=self, text='カレントフォルダにある全てのinpファイルを変換する', tgt=form.CONVALLKw, sel=0)
        l = FXLabel(p=self, text='パラメータ設定：', opts=JUSTIFY_LEFT)
        l.setFont( getAFXFont(FONT_BOLD) )
        
        TabBook_1 = FXTabBook(p=self, tgt=None, sel=0,
            opts=TABBOOK_NORMAL|LAYOUT_FILL_X,
            x=0, y=0, w=0, h=0, pl=DEFAULT_SPACING, pr=DEFAULT_SPACING,
            pt=DEFAULT_SPACING, pb=DEFAULT_SPACING)
        tabItem = FXTabItem(p=TabBook_1, text='共通', ic=None, opts=TAB_TOP_NORMAL,
            x=0, y=0, w=0, h=0, pl=6, pr=6, pt=DEFAULT_PAD, pb=DEFAULT_PAD)
        TabItem_1 = FXVerticalFrame(p=TabBook_1,
            opts=FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X,
            x=0, y=0, w=0, h=0, pl=DEFAULT_SPACING, pr=DEFAULT_SPACING,
            pt=DEFAULT_SPACING, pb=DEFAULT_SPACING, hs=DEFAULT_SPACING, vs=DEFAULT_SPACING)
        VFrame_1 = FXVerticalFrame(p=TabItem_1, opts=LAYOUT_FILL_X, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0)
        HFrame_1 = FXHorizontalFrame(p=VFrame_1, opts=0, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0)
        FXCheckButton(p=HFrame_1, text='安定化機能を使用する', tgt=form.IS_STAB_ACTIVEKw, sel=0)
        AFXTextField(p=HFrame_1, ncols=12, labelText='安定化係数：', tgt=form.STABKw, sel=0)
        
        l = FXLabel(p=VFrame_1, text='ステップ制御：', opts=JUSTIFY_LEFT)
        vf = FXVerticalFrame(VFrame_1, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X,
            0,0,0,0, 0,0,0,0)
        vf.setSelector(99)
        table = AFXTable(vf, 4, 6, 4, 6, form.STEPCTRLKw, 0, AFXTABLE_EDITABLE)
        table.setLeadingRows(1)
        table.setLeadingColumns(1)
        table.setColumnWidth(1, 80)
        table.setColumnType(1, AFXTable.INT)
        table.setColumnWidth(2, 80)
        table.setColumnType(2, AFXTable.INT)
        table.setColumnWidth(3, 80)
        table.setColumnType(3, AFXTable.INT)
        table.setColumnWidth(4, 80)
        table.setColumnType(4, AFXTable.INT)
        table.setColumnWidth(5, 80)
        table.setColumnType(5, AFXTable.FLOAT)
        table.setLeadingRowLabels('ステップ\n分割数\t平滑化\tNewton法\n反復上限\t出力間隔\t収束閾値')
        table.setStretchableColumn( table.getNumColumns()-1 )
        table.showHorizontalGrid(True)
        table.showVerticalGrid(True)
        l = FXLabel(p=VFrame_1, text='※各ステップの解析および出力設定を行います。', opts=JUSTIFY_LEFT)
        l = FXLabel(p=VFrame_1, text='※3ステップ目以降のステップには、全て3行目の設定が使用されます。', opts=JUSTIFY_LEFT)
        l = FXLabel(p=VFrame_1, text='※入力しない場合はデフォルトの値が使用されます。', opts=JUSTIFY_LEFT)
        l = FXLabel(p=VFrame_1, text='※ステップ分割数：1、平滑化：0、Newton法：50、出力間隔：1', opts=JUSTIFY_LEFT)

        tabItem = FXTabItem(p=TabBook_1, text='接触', ic=None, opts=TAB_TOP_NORMAL,
            x=0, y=0, w=0, h=0, pl=6, pr=6, pt=DEFAULT_PAD, pb=DEFAULT_PAD)
        TabItem_2 = FXVerticalFrame(p=TabBook_1,
            opts=FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X,
            x=0, y=0, w=0, h=0, pl=DEFAULT_SPACING, pr=DEFAULT_SPACING,
            pt=DEFAULT_SPACING, pb=DEFAULT_SPACING, hs=DEFAULT_SPACING, vs=DEFAULT_SPACING)
        l = FXLabel(p=TabItem_2, text='接触反復数 (現在は無効): ', opts=JUSTIFY_LEFT)
        HFrame_5 = FXHorizontalFrame(p=TabItem_2, opts=LAYOUT_FILL_X, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0)
        AFXTextField(p=HFrame_5, ncols=4, labelText='初期 : ', tgt=form.AL1Kw, sel=0)
        AFXTextField(p=HFrame_5, ncols=4, labelText='中間 : ', tgt=form.AL2Kw, sel=0)
        AFXTextField(p=HFrame_5, ncols=4, labelText='最終 : ', tgt=form.AL3Kw, sel=0)
        l = FXLabel(p=TabItem_2, text='ペナルティ値 : ', opts=JUSTIFY_LEFT)
        HFrame_6 = FXHorizontalFrame(p=TabItem_2, opts=0, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0)
        AFXTextField(p=HFrame_6, ncols=10, labelText='接触 : ', tgt=form.PCNTKw, sel=0)
        AFXTextField(p=HFrame_6, ncols=10, labelText='結合 :', tgt=form.PTIEKw, sel=0)
        l = FXLabel(p=TabItem_2, text='接触安定化（効果範囲>0の時のみ有効）: ', opts=JUSTIFY_LEFT)
        HFrame_7 = FXHorizontalFrame(p=TabItem_2, opts=LAYOUT_FILL_X, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0)
        AFXTextField(p=HFrame_7, ncols=6, labelText='効果範囲 : ', tgt=form.CONTSTABLENGTHKw, sel=0)
        AFXTextField(p=HFrame_7, ncols=8, labelText='減衰係数 : ', tgt=form.CONTSTABFACTORKw, sel=0)
        FXCheckButton(p=TabItem_2, text='干渉チェッカー用デバッグデータを出力する', tgt=form.OPCOLDATKw, sel=0)
        l = FXLabel(p=TabItem_2, text='　（FSTR.dbg.*にデータが出力されます。） ', opts=JUSTIFY_LEFT)

        tabItem = FXTabItem(p=TabBook_1, text='結果出力', ic=None, opts=TAB_TOP_NORMAL,
            x=0, y=0, w=0, h=0, pl=6, pr=6, pt=DEFAULT_PAD, pb=DEFAULT_PAD)
        TabItem_3 = FXVerticalFrame(p=TabBook_1,
            opts=FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X,
            x=0, y=0, w=0, h=0, pl=DEFAULT_SPACING, pr=DEFAULT_SPACING,
            pt=DEFAULT_SPACING, pb=DEFAULT_SPACING, hs=DEFAULT_SPACING, vs=DEFAULT_SPACING)
        VFrame_1 = FXVerticalFrame(p=TabItem_3, opts=LAYOUT_FILL_X, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0)
        GroupBox_2 = FXGroupBox(p=VFrame_1, text='', opts=FRAME_GROOVE|LAYOUT_FILL_X)
        FXCheckButton(p=GroupBox_2, text='デフォルトのAVSファイルを出力する', tgt=form.OPAVSKw, sel=0)
#        AFXTextField(p=GroupBox_2, ncols=4, labelText='出力間隔 (vtk のみ有効)', tgt=form.OPFREQKw, sel=0)
        l = FXLabel(p=GroupBox_2, text='出力変数 (0.logにのみ有効):', opts=JUSTIFY_LEFT)
        l.setFont( getAFXFont(FONT_BOLD) )
        l = FXLabel(p=GroupBox_2, text='節点値：', opts=JUSTIFY_LEFT)
        HFrame_2 = FXHorizontalFrame(p=GroupBox_2, opts=LAYOUT_FILL_X, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0)
        FXCheckButton(p=HFrame_2, text='変位', tgt=form.LDISPKw, sel=0)
        FXCheckButton(p=HFrame_2, text='反力', tgt=form.LREACKw, sel=0)
        FXCheckButton(p=HFrame_2, text='歪み', tgt=form.LNSTRAINKw, sel=0)
        FXCheckButton(p=HFrame_2, text='応力', tgt=form.LNSTRESSKw, sel=0)
        l = FXLabel(p=GroupBox_2, text='要素値：', opts=JUSTIFY_LEFT)
        HFrame_3 = FXHorizontalFrame(p=GroupBox_2, opts=0, x=0, y=0, w=0, h=0,
            pl=0, pr=0, pt=0, pb=0)
        FXCheckButton(p=HFrame_3, text='歪み', tgt=form.LESTRAINKw, sel=0)
        FXCheckButton(p=HFrame_3, text='応力', tgt=form.LESTRESSKw, sel=0)
        FXCheckButton(p=HFrame_3, text='塑性歪み', tgt=form.LPLSTKw, sel=0)

###########################################################################
# Class definition
###########################################################################

class a2FconverterDBFileHandler(FXObject):

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def __init__(self, form, keyword, patterns='*'):

        self.form = form
        self.patterns = patterns
        self.patternTgt = AFXIntTarget(0)
        exec('self.fileNameKw = form.%sKw' % keyword)
        self.readOnlyKw = AFXBoolKeyword(None, 'readOnly', AFXBoolKeyword.TRUE_FALSE)
        FXObject.__init__(self)
        FXMAPFUNC(self, SEL_COMMAND, AFXMode.ID_ACTIVATE, a2FconverterDBFileHandler.activate)

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def activate(self, sender, sel, ptr):

       fileDb = AFXFileSelectorDialog(getAFXApp().getAFXMainWindow(), 'Select a File',
           self.fileNameKw, self.readOnlyKw,
           AFXSELECTFILE_ANY, self.patterns, self.patternTgt)
       fileDb.setReadOnlyPatterns('*.odb')
       fileDb.create()
       fileDb.showModal()
