#include "Combat/Core/BtlBoss.h"

char Path_BtlBoss16b[0x1C] = "Apl_Mor/Grp_BtlBoss16b.bin";
char Path_BtlBoss15[0x1C]  = "Apl_Mor/Grp_BtlBoss15.bin";
char Path_BtlBoss16[0x1C]  = "Apl_Mor/Grp_BtlBoss16.bin";
char Path_BtlBoss15b[0x1C] = "Apl_Mor/Grp_BtlBoss15b.bin";

const BinIdentifier  Iden_BtlBoss16  = {3, Path_BtlBoss16};
const BinIdentifier  Iden_BtlBoss16b = {3, Path_BtlBoss16b};
const BinIdentifier  Iden_BtlBoss15  = {3, Path_BtlBoss15};
const Ov003BinIdenEx Iden_BtlBoss15b = {
    {3, Path_BtlBoss15b},
    {0x14, 0, 0, 1}
};
