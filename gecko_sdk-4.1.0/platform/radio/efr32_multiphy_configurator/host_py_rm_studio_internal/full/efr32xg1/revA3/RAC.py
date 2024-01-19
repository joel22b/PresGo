
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_RAC' ]

from . static import Base_RM_Peripheral
from . RAC_register import *

class RM_Peripheral_RAC(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_RAC, self).__init__(rmio, label,
            0x40084000, 'RAC',
            u"",
            [])
        self.RXENSRCEN = RM_Register_RAC_RXENSRCEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['RXENSRCEN'] = self.RXENSRCEN
        self.STATUS = RM_Register_RAC_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.CMD = RM_Register_RAC_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.CTRL = RM_Register_RAC_CTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL'] = self.CTRL
        self.FORCESTATE = RM_Register_RAC_FORCESTATE(self.zz_rmio, self.zz_label)
        self.zz_rdict['FORCESTATE'] = self.FORCESTATE
        self.IF = RM_Register_RAC_IF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IF'] = self.IF
        self.IFS = RM_Register_RAC_IFS(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFS'] = self.IFS
        self.IFC = RM_Register_RAC_IFC(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFC'] = self.IFC
        self.IEN = RM_Register_RAC_IEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IEN'] = self.IEN
        self.LVDSCTRL = RM_Register_RAC_LVDSCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['LVDSCTRL'] = self.LVDSCTRL
        self.LVDSIDLESEQ = RM_Register_RAC_LVDSIDLESEQ(self.zz_rmio, self.zz_label)
        self.zz_rdict['LVDSIDLESEQ'] = self.LVDSIDLESEQ
        self.LVDSROUTE = RM_Register_RAC_LVDSROUTE(self.zz_rmio, self.zz_label)
        self.zz_rdict['LVDSROUTE'] = self.LVDSROUTE
        self.HFXORETIMECTRL = RM_Register_RAC_HFXORETIMECTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['HFXORETIMECTRL'] = self.HFXORETIMECTRL
        self.HFXORETIMESTATUS = RM_Register_RAC_HFXORETIMESTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['HFXORETIMESTATUS'] = self.HFXORETIMESTATUS
        self.TESTCTRL = RM_Register_RAC_TESTCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['TESTCTRL'] = self.TESTCTRL
        self.SEQSTATUS = RM_Register_RAC_SEQSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQSTATUS'] = self.SEQSTATUS
        self.SEQCMD = RM_Register_RAC_SEQCMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQCMD'] = self.SEQCMD
        self.BREAKPOINT = RM_Register_RAC_BREAKPOINT(self.zz_rmio, self.zz_label)
        self.zz_rdict['BREAKPOINT'] = self.BREAKPOINT
        self.R0 = RM_Register_RAC_R0(self.zz_rmio, self.zz_label)
        self.zz_rdict['R0'] = self.R0
        self.R1 = RM_Register_RAC_R1(self.zz_rmio, self.zz_label)
        self.zz_rdict['R1'] = self.R1
        self.R2 = RM_Register_RAC_R2(self.zz_rmio, self.zz_label)
        self.zz_rdict['R2'] = self.R2
        self.R3 = RM_Register_RAC_R3(self.zz_rmio, self.zz_label)
        self.zz_rdict['R3'] = self.R3
        self.R4 = RM_Register_RAC_R4(self.zz_rmio, self.zz_label)
        self.zz_rdict['R4'] = self.R4
        self.R5 = RM_Register_RAC_R5(self.zz_rmio, self.zz_label)
        self.zz_rdict['R5'] = self.R5
        self.R6 = RM_Register_RAC_R6(self.zz_rmio, self.zz_label)
        self.zz_rdict['R6'] = self.R6
        self.R7 = RM_Register_RAC_R7(self.zz_rmio, self.zz_label)
        self.zz_rdict['R7'] = self.R7
        self.WAITMASK = RM_Register_RAC_WAITMASK(self.zz_rmio, self.zz_label)
        self.zz_rdict['WAITMASK'] = self.WAITMASK
        self.WAITSNSH = RM_Register_RAC_WAITSNSH(self.zz_rmio, self.zz_label)
        self.zz_rdict['WAITSNSH'] = self.WAITSNSH
        self.STIMER = RM_Register_RAC_STIMER(self.zz_rmio, self.zz_label)
        self.zz_rdict['STIMER'] = self.STIMER
        self.STIMERCOMP = RM_Register_RAC_STIMERCOMP(self.zz_rmio, self.zz_label)
        self.zz_rdict['STIMERCOMP'] = self.STIMERCOMP
        self.VECTADDR = RM_Register_RAC_VECTADDR(self.zz_rmio, self.zz_label)
        self.zz_rdict['VECTADDR'] = self.VECTADDR
        self.SEQCTRL = RM_Register_RAC_SEQCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQCTRL'] = self.SEQCTRL
        self.PRESC = RM_Register_RAC_PRESC(self.zz_rmio, self.zz_label)
        self.zz_rdict['PRESC'] = self.PRESC
        self.SR0 = RM_Register_RAC_SR0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SR0'] = self.SR0
        self.SR1 = RM_Register_RAC_SR1(self.zz_rmio, self.zz_label)
        self.zz_rdict['SR1'] = self.SR1
        self.SR2 = RM_Register_RAC_SR2(self.zz_rmio, self.zz_label)
        self.zz_rdict['SR2'] = self.SR2
        self.SR3 = RM_Register_RAC_SR3(self.zz_rmio, self.zz_label)
        self.zz_rdict['SR3'] = self.SR3
        self.SYNTHENCTRL = RM_Register_RAC_SYNTHENCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNTHENCTRL'] = self.SYNTHENCTRL
        self.SYNTHREGCTRL = RM_Register_RAC_SYNTHREGCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNTHREGCTRL'] = self.SYNTHREGCTRL
        self.VCOCTRL = RM_Register_RAC_VCOCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['VCOCTRL'] = self.VCOCTRL
        self.MMDCTRL = RM_Register_RAC_MMDCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['MMDCTRL'] = self.MMDCTRL
        self.CHPCTRL = RM_Register_RAC_CHPCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHPCTRL'] = self.CHPCTRL
        self.CHPCAL = RM_Register_RAC_CHPCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHPCAL'] = self.CHPCAL
        self.LPFCTRL = RM_Register_RAC_LPFCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['LPFCTRL'] = self.LPFCTRL
        self.SYNTHCTRL = RM_Register_RAC_SYNTHCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNTHCTRL'] = self.SYNTHCTRL
        self.AUXCTRL = RM_Register_RAC_AUXCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXCTRL'] = self.AUXCTRL
        self.AUXENCTRL = RM_Register_RAC_AUXENCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXENCTRL'] = self.AUXENCTRL
        self.RFENCTRL = RM_Register_RAC_RFENCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFENCTRL'] = self.RFENCTRL
        self.RFENCTRL0 = RM_Register_RAC_RFENCTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFENCTRL0'] = self.RFENCTRL0
        self.LNAMIXCTRL = RM_Register_RAC_LNAMIXCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXCTRL'] = self.LNAMIXCTRL
        self.PACTRL0 = RM_Register_RAC_PACTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PACTRL0'] = self.PACTRL0
        self.PAPKDCTRL = RM_Register_RAC_PAPKDCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['PAPKDCTRL'] = self.PAPKDCTRL
        self.PABIASCTRL0 = RM_Register_RAC_PABIASCTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PABIASCTRL0'] = self.PABIASCTRL0
        self.PABIASCTRL1 = RM_Register_RAC_PABIASCTRL1(self.zz_rmio, self.zz_label)
        self.zz_rdict['PABIASCTRL1'] = self.PABIASCTRL1
        self.SGRFENCTRL0 = RM_Register_RAC_SGRFENCTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SGRFENCTRL0'] = self.SGRFENCTRL0
        self.SGLNAMIXCTRL = RM_Register_RAC_SGLNAMIXCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SGLNAMIXCTRL'] = self.SGLNAMIXCTRL
        self.SGPACTRL0 = RM_Register_RAC_SGPACTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SGPACTRL0'] = self.SGPACTRL0
        self.SGPAPKDCTRL = RM_Register_RAC_SGPAPKDCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SGPAPKDCTRL'] = self.SGPAPKDCTRL
        self.SGPABIASCTRL0 = RM_Register_RAC_SGPABIASCTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SGPABIASCTRL0'] = self.SGPABIASCTRL0
        self.SGPABIASCTRL1 = RM_Register_RAC_SGPABIASCTRL1(self.zz_rmio, self.zz_label)
        self.zz_rdict['SGPABIASCTRL1'] = self.SGPABIASCTRL1
        self.RFSTATUS = RM_Register_RAC_RFSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFSTATUS'] = self.RFSTATUS
        self.RFBIASCTRL = RM_Register_RAC_RFBIASCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFBIASCTRL'] = self.RFBIASCTRL
        self.RFBIASCAL = RM_Register_RAC_RFBIASCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFBIASCAL'] = self.RFBIASCAL
        self.LNAMIXCTRL1 = RM_Register_RAC_LNAMIXCTRL1(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXCTRL1'] = self.LNAMIXCTRL1
        self.IFPGACTRL = RM_Register_RAC_IFPGACTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFPGACTRL'] = self.IFPGACTRL
        self.IFPGACAL = RM_Register_RAC_IFPGACAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFPGACAL'] = self.IFPGACAL
        self.IFFILTCTRL = RM_Register_RAC_IFFILTCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFFILTCTRL'] = self.IFFILTCTRL
        self.IFADCCTRL = RM_Register_RAC_IFADCCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCCTRL'] = self.IFADCCTRL
        self.IFADCCAL = RM_Register_RAC_IFADCCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCCAL'] = self.IFADCCAL
        self.PAENCTRL = RM_Register_RAC_PAENCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['PAENCTRL'] = self.PAENCTRL
        self.PACTUNECTRL = RM_Register_RAC_PACTUNECTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['PACTUNECTRL'] = self.PACTUNECTRL
        self.RCTUNE = RM_Register_RAC_RCTUNE(self.zz_rmio, self.zz_label)
        self.zz_rdict['RCTUNE'] = self.RCTUNE
        self.APC = RM_Register_RAC_APC(self.zz_rmio, self.zz_label)
        self.zz_rdict['APC'] = self.APC
        self.SPARE = RM_Register_RAC_SPARE(self.zz_rmio, self.zz_label)
        self.zz_rdict['SPARE'] = self.SPARE
        self.__dict__['zz_frozen'] = True