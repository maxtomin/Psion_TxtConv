bmconv convmbm.lst
cpp -I\Epoc32\Include\ <TextConv.rss >TextConv.rpp
rcomp -oTextConv.rsc -hTextConv.rsg -sTextConv.rpp -iTextConv.rss

copy TextConv.rsc aif\aif.rsc
copy TextConv.mbm aif\aif.mbm
call \epoc32\release\winc\deb\aiftool.exe C:\psiconv\TextConverter\aif\aif\
copy aif\out.aif TextConverter.aif