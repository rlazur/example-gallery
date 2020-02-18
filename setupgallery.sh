#use ups to probe which versions of these products exist
#for example, if I wanted to know which versions of icaruscode is available I would do
#     ups list -aK+ icaruscode
setup icaruscode v08_37_00 -q e17:prof
setup gallery v1_13_01 -q e17:prof
setup cmake v3_10_1
