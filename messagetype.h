#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

#define SERVERID 0
#define REQID 0
#define ERRORID 0xffffffff

#define LOGINEVENTGROUP 0x0
#define REQINFOGROUP 0x1
#define CHANGEINFOGROUP 0x2
#define REQUDPEVENTGROUP 0x3
#define TRANSFERDATAGROUP 0x4
#define FILETRANSFERGROUP 0xfe
#define UDPEVENTGROUP 0xff


#define RESFILESENDETHREADRGROUP 0xfff
#define RESFILETRANSDERINFOGROUP 0xfffa
#define RESTRANSFERGROUP 0xfffb
#define RESUDPREQGROUP 0xfffc
#define RESCHANGEGROUP 0xfffd
#define RESINFOGROUP 0xfffe
#define SYSRESEVENTGROUP 0xffff



//0x0
#define LOGINACTION 0x0
#define REGISTERACCOUNTACTION 0x1
#define SOCKETMERGEACTION 0xfff
#define SYSTESTLOGACTION 0xffff


//0x1
#define REQUSERINFOACTION 0x0
#define REQFRIENDSGROUPACTION 0x1
#define REQFRIENDSACTION 0x2
#define REQUSERSGROUPSACTION 0x3
#define REQUSERSGROUPINFOACTION 0x4
#define REQUSERSGROUPMEMBERACTION 0x5
#define REQUSERFILESINFOACTION 0x6



//0x2
#define CHANGEFRIENDOTHERNAMEACTION 0x0
#define ADDFRIENDSGROUPACTION 0x1
#define DELFRIENDSACTION 0x2
#define CHANGEFRIENDSGROUPACTION 0x3
#define DELUSERSGROUPSACTION 0x4
#define DELUSERSGROUPMEMBERSACTION 0x5
#define CHANGEUSERSGROUPINFO 0x6
#define CHANGEMYSELFINFOACTION 0x7
#define CREATENEWUSERSGROUPACTION 0x8
#define REQREALADDFRIENDACTION 0x9
#define ADDMEMBERTOUSERSGROUP 0xa

//0x3
#define REQUDPCHATACTION 0x0
#define REQCLOSEUDPCHATACTION 0x1
#define REQUDPCHATSUCCESSACTION 0x2

//0x4
#define TRANSFERCHATDATAACTION 0x0
#define TRANSFERREQFILESENDACTION 0x1
#define RESTRANSFERFILEACTION 0x2
#define TRANSFERFILEENDACTION 0x3
#define TRANSFERFILECONTINUE 0x4
#define ADDREMOTEFRIENDACTION 0x5
#define REQJOININGROUPACTION 0x6
#define REQFORCECLOSEFILEACTION 0x7

//0xfe
#define UPLOADFILEDATAACTION 0x0
#define READYSENDPROFILEACTION 0x1
#define WRITEPROFILEENDACTION 0x2
#define REQREADFILESOPENACTION 0x3
#define REQREADFILEDATAACTION 0x4
#define REQREADYSENDUSERFILEACTION 0x5
#define SENDUSERFILEENDACTION 0x6



//0xff
#define UDPADDENDPOINT 0x0
#define UDPVIDEODATA 0x1
#define UDPAUDIODATA 0x2
#define UDPTRANSFERMSG 0x8000
#define UDPVIDEOTRANSFERDATA 0x8001
#define UDPAUDIOTRANSFERDATA 0x8002


//0xfff
#define RESREADFILEDATAACTION 0x0
#define TRANSFERFILEDATAACTION 0x1


//0xfffa
#define RESREADYPROFILETRANSER 0x0
#define RESUPLOADDATACOUNTCODE 0x1
#define RESUPLOADSUCCESSCODE 0x2
#define RESREADFILESOPENCODE 0x3
#define RESREADFILEENDCODE 0x4
#define RESREDAYSENDORDOWNLOADUSERFILECODE 0x5
#define RESSENDUSERFILEENDACODE 0x6


//0xfffb
#define RESTRANSFERERROR 0x0


//0xfffc
#define RESREADYUDPCHATSENDCODE 0x0
#define RESCLOSEUDPCHATCODE 0x1
#define RESSENDUDPENDPOINTCODE 0x2


//0xfffd
#define RESCHANGEFRIENDOTHERNAMECODE 0x0
#define RESADDFRIENDSGROUPCODE 0x1
#define RESDELFRIENDSCODE 0x2
#define RESCHANGEFRIENDSGROUPCODE 0x3
#define RESDELUSERSGROUPSOKCODE 0x4
#define RESDELBYADMINCODE 0x5
#define RESDELMEMBERSUCCEESCODE 0x6
#define RESCHANGEUSERSGROUPINFOCODE 0x7
#define RESCHANGEMYSELFINFOCODE 0x8
#define RESCREATENEWUSERSGROUPCODE 0x9
#define RESREALADDFRIENDCODE 0xa
#define RESADDMEMBERTOGROUPCODE 0xb


//0xfffe
#define RESUSERINFOCODE 0x0
#define RESUSERSGROUPINFOCODE 0x1
#define RESUSERSGROUPMEMBERCODE 0x2
#define RESMERGESUCCESSCODE 0x3
#define RESUSERFILESINFOCODE 0x4




//0xffff
#define SYSRESSUCCESSCODE 0x0
#define RESREGISTERCODE 0x1
#define SYSRESERRORCODE 0xffff




#define MESSAGETYPE(t1, t2) (((t1) << 16) | (t2))
#define TYPEGROUP(t) ((t) >> 16)
#define TYPEACTION(t) ((t) & 0xffff)

//上传文件类型
#define UPLOADUSERPROFILE 0
#define UPLOADUSERSGROUPPROFILE 1
#define DOWNLOADUSERPROFILE 2

#define UPLOADUSERFILE 4
#define DOWNLOADUSERFILE 5
#define FILEDATATRANSFER 6




#endif
