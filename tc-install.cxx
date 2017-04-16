// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include <libintl.h>
#include "tc-install.h"
// (c) Robert Shingledecker 2011
// Portions (c) Brian Smith 2011
// Refactor (c) polikuo 2017
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_message.H>
#include <locale.h>
using namespace std;
static istringstream iss; 
static string command,msg,heading; 
static int results, locales_set=0, test4syslinux, test4mkdosfs, test4perl; 
static string image, type, installMode, target, dev, format, path, coreplus, boot, flag, markActive, tcepath; 
static string warn="Mark active if Tiny Core is the only operation system,\nor Windows and Tiny Core are on the same disk.\nOtherwise use Grub extension after installation."; 

void selectFile() {
  brwTarget->deactivate();
  string target = "core*.gz";
  string title = gettext("Select File for ");
  title = title + "core,corepure64";
  Fl_File_Chooser chooser("/",target.c_str(), Fl_File_Chooser::SINGLE, title.c_str());
  chooser.show();
  while(chooser.shown())
    { Fl::wait(); }
  
  if ( chooser.value() == NULL )
  {
     return;
  }
  
  path = chooser.value();
  Fl::flush();
}

void prepTarget() {
  if (path.empty()) return;
  
  btnActive->deactivate();
  btnBootloader->deactivate();
  if ( installMode =="hdd" || installMode == "zip" )
  {
     test4mkdosfs = system("which mkdosfs >/dev/null");
  
     if ( test4mkdosfs != 0 )
     {
        fl_message("HDD and ZIP require dosfstools extension be loaded!\nZIP also requires perl extension.");
        exit(1);
     }
  
     if ( installMode == "zip")
     {
        test4perl = system("which perl >/dev/null");
        if ( test4perl != 0 )
        {
           fl_message("ZIP installation requires perl extension be loaded.");
           exit(1);
        }
     }
  
     btnWhole->value(1);
     btnPartition->value(0);
     btnPartition->deactivate();
     type = "disk";
  } else {
     btnPartition->activate();
  }
  
  heading = "   Select " + type + " for " + image + "   "; // Add empty spaces to prevent glitches
  brwTarget->label(heading.c_str());
  
  flag = "x";
  if ( type == "disk" )
     flag = "-d";
  if (type == "partition" )
     flag = "-p";
  
  if (! image.empty() && flag != "x")
  {
      brwTarget->activate();
      command = "fetch_devices " + flag + " > /tmp/install_work";
      system(command.c_str());
      brwTarget->load("/tmp/install_work");
      unlink("/tmp/install_work");
  }
  grpButtons->deactivate();
}

void cursor_normal() {
  window->cursor(FL_CURSOR_DEFAULT);
  Fl::flush();
}

void cursor_wait() {
  window->cursor(FL_CURSOR_WAIT);
  Fl::flush();
}

static char * mygettext(const char *msgid) {
  if (!locales_set) {
  
  setlocale(LC_ALL, "");
  bindtextdomain("tinycore","/usr/local/share/locale");
  textdomain("tinycore");
  
  locales_set=1;
  
  }
  
  return gettext(msgid);
}

void btnCB(Fl_Widget*, void*userdata) {
  const string userdatastr = userdata ? (char *) userdata : "";
  
  /*if ( userdatastr == "begin" )
  {
    wWizard->value(wWizard->child(0));
  } */
  
  if ( userdatastr == "next" )
  {
    if ( grpBoot->visible() ){
     	string command = "grep 'Core Plus' `dirname " + path + "`/isolinux/isolinux.cfg >/dev/null 2>&1";
     	int results = system(command.c_str());
     	if (results == 0){
     	     coreplus = "yes";
     	     wWizard->value(grpExtensionsCorePlus);
     	}else{
     	     coreplus = "no";
     	     wWizard->value(grpExtensionsStandAlone);
     	}
    }else if (grpExtensionsCorePlus->visible() ){
          wWizard->value(grpReview);
    }else{
  	  wWizard->next();
  	  if ( grpFormat->visible() && installMode == "zip")
  	     wWizard->next();
  
  	  if ( btnWhole->value() == 1 ){
  	     btnNoFormat->deactivate();
  	     btnNoFormat->value(0);
  	  }else{
  	     btnNoFormat->activate();
  	  }
  
  	  if (btnNoFormat->value() == 0 && btnExt2Format->value() == 0 && btnExt3Format->value() == 0 && btnExt4Format->value() == 0 && btnVfatFormat->value() == 0){
               btnExt4Format->value(1);
               format = "ext4";
            }
  
    }
  }
  
  if ( userdatastr == "prev" )
  {
    if (grpExtensionsStandAlone->visible() ) {
    	wWizard->value(grpBoot);
    }else if (grpReview->visible() ){
     	string command = "grep 'Core Plus' `dirname " + path + "`/isolinux/isolinux.cfg >/dev/null 2>&1";
     	int results = system(command.c_str());
     	if (results == 0){
     	     wWizard->value(grpExtensionsCorePlus);
     	}else{
     	     wWizard->value(grpExtensionsStandAlone);
     	}
    }else{
  	  wWizard->prev();
  	  if ( grpFormat->visible() && installMode == "zip")
  	     wWizard->prev();
    }
  }
  
  /*if ( userdatastr == "last" )
  {
    int last = wWizard->children()-1;
    wWizard->value(wWizard->child(last));
  }*/
  
  if (grpType->visible())
     prepTarget();
  
  if (grpBoot->visible())
  {
     /*if ( installMode =="hdd" || installMode == "zip" )
        options->value("waitusb=5");*/
  
     brwBootRef->load("/usr/share/doc/tc/bootOptions.txt");
  }
  
  if (grpReview->visible())
  {
     markActive="0";
     boot = options->value();
     brwReview->clear();
     brwReview->add(("Source: "+path).c_str());
     brwReview->add(("Type: "+installMode).c_str());
     brwReview->add(("Target: "+dev).c_str());
     if (btnActive->value() == 1)
     {
        brwReview->add("Mark partition active (bootable)");
        markActive="1";
     }
     if ( installMode != "zip" )
        brwReview->add(("Format:"+format).c_str());
     brwReview->add(("Options: "+boot).c_str());
  
     if (coreplus == "yes"){
     	if (installCoreX11->value() == 1) {brwReview->add("Install X GUI");}
   	if (installCoreOnly->value() == 1) {brwReview->add("Install Core Only (text mode)");}
   	if (btnWifi->value() == 1) {brwReview->add("Install Wifi Support");}
   	if (btnNdiswrapper->value() == 1) {brwReview->add("Install ndiswrapper");}
   	if (btnWifiFirmware->value() == 1) {brwReview->add("Install Wireless Firmware");}
   	if (btnInstaller->value() == 1) {brwReview->add("Install Installer Application");}
   	if (btnRemaster->value() == 1) {brwReview->add("Install Remaster Tool");}
   	if (btnKmaps->value() == 1) {brwReview->add("Install Support for Non-US keyboard maps");}
     }else{
     	if (! tcepath.empty()) {brwReview->add(("Install TCE/CDE Dircetory: "+tcepath).c_str());}
     }
  }
}

void brwCB(Fl_Widget*) {
  if ( brwTarget->value() )
  {
    dev = brwTarget->text(brwTarget->value());
    dev = dev.substr(0,dev.find("\t",0));
    int partitionNbr;
    iss.str(dev.substr((dev.length() - 1),dev.length()));
    iss >> partitionNbr;
    grpButtons->activate();
    if (btnPartition->value() == 1  && partitionNbr < 5)
      btnActive->activate();
    if (installMode == "frugal")
      btnBootloader->activate();
    else
      btnBootloader->value(1);
  }
}

void btnProceedCB(Fl_Widget*) {
  btnProceed->deactivate();
  grpButtons->deactivate();
  cursor_wait();
  
  string coreplusinstalltype;
  if (installCoreX11->value() == 1) {
  	coreplusinstalltype = "X";
  }else{
  	coreplusinstalltype = "Core";
  }
  
  string installgroups = "";
  if (btnWifi->value() == 1){
      installgroups = installgroups + ",wifi";
  }
  if (btnNdiswrapper->value() == 1){
      installgroups = installgroups + ",ndiswrapper";
  }
  if (btnWifiFirmware->value() == 1){
      installgroups = installgroups + ",wififirmware";
  }
  if (btnInstaller->value() == 1){
      installgroups = installgroups + ",installer";
  }
  if (btnRemaster->value() == 1){
      installgroups = installgroups + ",remaster";
  }
  if (btnKmaps->value() == 1){
      installgroups = installgroups + ",kmaps";
  }
  if (installgroups.empty()) {installgroups = "none";}
  
  string bootLoader = "no";
  if (btnBootloader->value() == 1) { bootLoader = "yes"; }
  
  string tcedir = outputTCEDir->value();
  if (tcedir.empty()) { tcedir = "none"; }
  if (btnNoExtensions->value() == 1) { tcedir = "none"; }
  
  command="sudo tc-install.sh "+path+" "+installMode+" "+dev+" "+markActive+" "+format+" "+bootLoader+" "+coreplus+" "+coreplusinstalltype+" "+installgroups+" "+tcedir+" "+boot;
  FILE *pipe = popen(command.c_str(),"r");
  char *mbuf = (char *)calloc(PATH_MAX,sizeof(char));
  if (pipe)
  {
     brwReview->clear();
     while(fgets(mbuf,PATH_MAX,pipe))
     {
        string line(mbuf);
        brwReview->add(line.c_str());
        Fl::flush();
        brwReview->bottomline(brwReview->size());
     }
     pclose(pipe);
     free(mbuf);
  }
  cursor_normal();
  Fl::flush();
}

void choose_arch() {
  switch (fl_choice("Which architecture?", "Cancel", "64-bit", "32-bit")) {
    case 2:
    image = "core.gz";
    path = "/tmp/net_source/core.gz";
    fullpathOutput->value(path.c_str());
    break;
    case 1:
    image = "corepure64.gz";
    path = "/tmp/net_source/corepure64.gz";
    fullpathOutput->value(path.c_str());
    break;
    default:
    fullpathOutput->activate();
    fullpathOutput->value("");
    net_mode->value(0);
    image.clear();
    path.clear();
    btnBootloader->deactivate();
    grpButtons->deactivate();
  }
}

Fl_Double_Window *window=(Fl_Double_Window *)0;

Fl_Wizard *wWizard=(Fl_Wizard *)0;

Fl_Group *grpType=(Fl_Group *)0;

Fl_Box *lblPathCore=(Fl_Box *)0;

Fl_Output *fullpathOutput=(Fl_Output *)0;

static void cb_fullpathOutput(Fl_Output*, void* v) {
  image = (const char*)v;
selectFile();
fullpathOutput->value(path.c_str());
prepTarget();
}

static void cb_Frugal(Fl_Check_Button*, void* v) {
  installMode = (const char*)v;
prepTarget();
}

static void cb_USB(Fl_Check_Button*, void* v) {
  installMode = (const char*)v;
prepTarget();
}

static void cb_USB1(Fl_Check_Button*, void* v) {
  installMode = (const char*)v;
prepTarget();
}

Fl_Check_Button *btnWhole=(Fl_Check_Button *)0;

static void cb_btnWhole(Fl_Check_Button*, void* v) {
  if (btnWhole->value() == 1 )
{
    type = (const char*)v;
    prepTarget();
};
}

Fl_Check_Button *btnPartition=(Fl_Check_Button *)0;

static void cb_btnPartition(Fl_Check_Button*, void* v) {
  if (btnPartition->value() == 1)
{
type = (const char*)v;
prepTarget();
};
}

Fl_Browser *brwTarget=(Fl_Browser *)0;

Fl_Check_Button *btnActive=(Fl_Check_Button *)0;

static void cb_btnActive(Fl_Check_Button*, void*) {
  if (btnActive->value() == 1)
{
   fl_message(warn.c_str());
};
}

Fl_Check_Button *btnBootloader=(Fl_Check_Button *)0;

Fl_Check_Button *net_mode=(Fl_Check_Button *)0;

static void cb_net_mode(Fl_Check_Button*, void*) {
  if (net_mode->value() == 1) {
  fullpathOutput->deactivate();
  choose_arch();
  prepTarget();
} else {
  brwTarget->deactivate();
  fullpathOutput->activate();
  fullpathOutput->value("");
  image.clear();
  path.clear();
  btnActive->deactivate();
  btnBootloader->deactivate();
  grpButtons->deactivate();
};
}

Fl_Group *grpFormat=(Fl_Group *)0;

Fl_Group *FormatType=(Fl_Group *)0;

Fl_Round_Button *btnNoFormat=(Fl_Round_Button *)0;

static void cb_btnNoFormat(Fl_Round_Button*, void* v) {
  format = (const char*)v;
}

Fl_Round_Button *btnExt2Format=(Fl_Round_Button *)0;

static void cb_btnExt2Format(Fl_Round_Button*, void* v) {
  format = (const char*)v;
}

Fl_Round_Button *btnExt3Format=(Fl_Round_Button *)0;

static void cb_btnExt3Format(Fl_Round_Button*, void* v) {
  format = (const char*)v;
}

Fl_Round_Button *btnExt4Format=(Fl_Round_Button *)0;

static void cb_btnExt4Format(Fl_Round_Button*, void* v) {
  format = (const char*)v;
}

Fl_Round_Button *btnVfatFormat=(Fl_Round_Button *)0;

static void cb_btnVfatFormat(Fl_Round_Button*, void* v) {
  format = (const char*)v;
}

Fl_Group *grpBoot=(Fl_Group *)0;

Fl_Browser *brwBootRef=(Fl_Browser *)0;

Fl_Input *options=(Fl_Input *)0;

Fl_Group *grpExtensionsCorePlus=(Fl_Group *)0;

Fl_Group *installType=(Fl_Group *)0;

Fl_Round_Button *installCoreX11=(Fl_Round_Button *)0;

Fl_Round_Button *installCoreOnly=(Fl_Round_Button *)0;

Fl_Group *corePlusExtensions=(Fl_Group *)0;

Fl_Check_Button *btnWifi=(Fl_Check_Button *)0;

Fl_Check_Button *btnNdiswrapper=(Fl_Check_Button *)0;

Fl_Check_Button *btnWifiFirmware=(Fl_Check_Button *)0;

Fl_Check_Button *btnInstaller=(Fl_Check_Button *)0;

Fl_Check_Button *btnRemaster=(Fl_Check_Button *)0;

Fl_Check_Button *btnKmaps=(Fl_Check_Button *)0;

Fl_Group *grpExtensionsStandAlone=(Fl_Group *)0;

Fl_Round_Button *btnInstallExtensions=(Fl_Round_Button *)0;

static void cb_btnInstallExtensions(Fl_Round_Button*, void*) {
  outputTCEDir->activate();
}

Fl_Round_Button *btnNoExtensions=(Fl_Round_Button *)0;

static void cb_btnNoExtensions(Fl_Round_Button*, void*) {
  outputTCEDir->deactivate();
}

Fl_Output *outputTCEDir=(Fl_Output *)0;

static void cb_outputTCEDir(Fl_Output*, void* v) {
  image = (const char*)v;

string target = "*";
string title = gettext("Select TCE/CDE Directory");
Fl_File_Chooser chooserTCE("/",target.c_str(), Fl_File_Chooser::DIRECTORY, title.c_str());
chooserTCE.show();
while(chooserTCE.shown())
  { Fl::wait(); }

if ( chooserTCE.value() == NULL )
{
   return;
}

tcepath = chooserTCE.value();
Fl::flush();

outputTCEDir->value(tcepath.c_str());
}

Fl_Group *grpReview=(Fl_Group *)0;

Fl_Browser *brwReview=(Fl_Browser *)0;

Fl_Button *btnProceed=(Fl_Button *)0;

Fl_Group *grpButtons=(Fl_Group *)0;

int main(int argc, char **argv) {
  { window = new Fl_Double_Window(480, 400, mygettext("Tiny Core Installation"));
    window->user_data((void*)("quit"));
    { wWizard = new Fl_Wizard(25, 30, 435, 325, mygettext("Tiny Core Installation"));
      wWizard->labeltype(FL_ENGRAVED_LABEL);
      { grpType = new Fl_Group(25, 30, 435, 325);
        { lblPathCore = new Fl_Box(55, 40, 105, 25, mygettext("Path to core.gz:"));
        } // Fl_Box* lblPathCore
        { fullpathOutput = new Fl_Output(55, 63, 375, 27);
          fullpathOutput->callback((Fl_Callback*)cb_fullpathOutput, (void*)("core"));
        } // Fl_Output* fullpathOutput
        { Fl_Group* o = new Fl_Group(55, 104, 375, 25);
          { Fl_Check_Button* o = new Fl_Check_Button(80, 105, 100, 22, mygettext("Frugal"));
            o->tooltip(mygettext("Use for frugal hard drive installations"));
            o->type(102);
            o->down_box(FL_DOWN_BOX);
            o->value(1);
            o->callback((Fl_Callback*)cb_Frugal, (void*)("frugal"));
            installMode = "frugal";
          } // Fl_Check_Button* o
          { Fl_Check_Button* o = new Fl_Check_Button(195, 105, 100, 22, mygettext("USB-HDD"));
            o->tooltip(mygettext("Use for pendrives.   Your BIOS must support USB-HDD booting"));
            o->type(102);
            o->down_box(FL_DOWN_BOX);
            o->callback((Fl_Callback*)cb_USB, (void*)("hdd"));
          } // Fl_Check_Button* o
          { Fl_Check_Button* o = new Fl_Check_Button(315, 105, 100, 22, mygettext("USB-ZIP"));
            o->tooltip(mygettext("Use for pendrives.   Drive will be formatted into two FAT partitions"));
            o->type(102);
            o->down_box(FL_DOWN_BOX);
            o->callback((Fl_Callback*)cb_USB1, (void*)("zip"));
          } // Fl_Check_Button* o
          o->end();
        } // Fl_Group* o
        { Fl_Group* o = new Fl_Group(55, 131, 375, 33);
          o->box(FL_ENGRAVED_FRAME);
          { btnWhole = new Fl_Check_Button(80, 136, 130, 25, mygettext("Whole Disk"));
            btnWhole->type(102);
            btnWhole->down_box(FL_DOWN_BOX);
            btnWhole->callback((Fl_Callback*)cb_btnWhole, (void*)("disk"));
          } // Fl_Check_Button* btnWhole
          { btnPartition = new Fl_Check_Button(255, 136, 130, 25, mygettext("Existing Partition"));
            btnPartition->type(102);
            btnPartition->down_box(FL_DOWN_BOX);
            btnPartition->callback((Fl_Callback*)cb_btnPartition, (void*)("partition"));
            //type = "partition";
          } // Fl_Check_Button* btnPartition
          o->end();
        } // Fl_Group* o
        { brwTarget = new Fl_Browser(55, 189, 375, 100, mygettext("Select Target Disk"));
          brwTarget->type(2);
          brwTarget->callback((Fl_Callback*)brwCB);
          brwTarget->align(Fl_Align(FL_ALIGN_TOP));
          brwTarget->deactivate();
        } // Fl_Browser* brwTarget
        { btnActive = new Fl_Check_Button(130, 293, 230, 24, mygettext("Mark Partition Active (bootable)"));
          btnActive->down_box(FL_DOWN_BOX);
          btnActive->callback((Fl_Callback*)cb_btnActive);
          btnActive->deactivate();
        } // Fl_Check_Button* btnActive
        { btnBootloader = new Fl_Check_Button(130, 318, 230, 24, mygettext("Install boot loader"));
          btnBootloader->down_box(FL_DOWN_BOX);
          btnBootloader->deactivate();
          btnBootloader->value(1);
        } // Fl_Check_Button* btnBootloader
        { net_mode = new Fl_Check_Button(255, 40, 175, 22, mygettext("Download From Internet"));
          net_mode->down_box(FL_DOWN_BOX);
          net_mode->callback((Fl_Callback*)cb_net_mode);
        } // Fl_Check_Button* net_mode
        grpType->end();
      } // Fl_Group* grpType
      { grpFormat = new Fl_Group(25, 30, 435, 325);
        grpFormat->hide();
        { FormatType = new Fl_Group(100, 95, 270, 230, mygettext("Formatting Options"));
          { btnNoFormat = new Fl_Round_Button(140, 105, 195, 20, mygettext("No formatting, use existing."));
            btnNoFormat->type(102);
            btnNoFormat->down_box(FL_ROUND_DOWN_BOX);
            btnNoFormat->value(1);
            btnNoFormat->callback((Fl_Callback*)cb_btnNoFormat, (void*)("none"));
            format = "none";
          } // Fl_Round_Button* btnNoFormat
          { btnExt2Format = new Fl_Round_Button(140, 125, 195, 20, mygettext("ext2"));
            btnExt2Format->type(102);
            btnExt2Format->down_box(FL_ROUND_DOWN_BOX);
            btnExt2Format->callback((Fl_Callback*)cb_btnExt2Format, (void*)("ext2"));
          } // Fl_Round_Button* btnExt2Format
          { btnExt3Format = new Fl_Round_Button(140, 145, 195, 20, mygettext("ext3"));
            btnExt3Format->type(102);
            btnExt3Format->down_box(FL_ROUND_DOWN_BOX);
            btnExt3Format->callback((Fl_Callback*)cb_btnExt3Format, (void*)("ext3"));
          } // Fl_Round_Button* btnExt3Format
          { btnExt4Format = new Fl_Round_Button(140, 165, 195, 20, mygettext("ext4"));
            btnExt4Format->type(102);
            btnExt4Format->down_box(FL_ROUND_DOWN_BOX);
            btnExt4Format->callback((Fl_Callback*)cb_btnExt4Format, (void*)("ext4"));
          } // Fl_Round_Button* btnExt4Format
          { btnVfatFormat = new Fl_Round_Button(140, 185, 195, 20, mygettext("vfat"));
            btnVfatFormat->type(102);
            btnVfatFormat->down_box(FL_ROUND_DOWN_BOX);
            btnVfatFormat->callback((Fl_Callback*)cb_btnVfatFormat, (void*)("vfat"));
          } // Fl_Round_Button* btnVfatFormat
          FormatType->end();
        } // Fl_Group* FormatType
        grpFormat->end();
      } // Fl_Group* grpFormat
      { grpBoot = new Fl_Group(25, 30, 435, 325, mygettext("Boot Options"));
        grpBoot->hide();
        { brwBootRef = new Fl_Browser(35, 55, 415, 235, mygettext("Boot Options Reference List"));
          brwBootRef->textfont(4);
          brwBootRef->align(Fl_Align(FL_ALIGN_TOP));
        } // Fl_Browser* brwBootRef
        { options = new Fl_Input(35, 325, 415, 20, mygettext("Enter Spaces Separated Options From Examples Above.\nThese can be edited late\
r on via your bootloader config."));
          options->labeltype(FL_EMBOSSED_LABEL);
          options->align(Fl_Align(FL_ALIGN_TOP));
        } // Fl_Input* options
        grpBoot->end();
      } // Fl_Group* grpBoot
      { grpExtensionsCorePlus = new Fl_Group(50, 55, 370, 285);
        grpExtensionsCorePlus->hide();
        { installType = new Fl_Group(85, 70, 310, 75, mygettext("Install Type"));
          { installCoreX11 = new Fl_Round_Button(138, 75, 238, 20, mygettext("Core and X/GUI Desktop"));
            installCoreX11->type(102);
            installCoreX11->down_box(FL_ROUND_DOWN_BOX);
            installCoreX11->value(1);
          } // Fl_Round_Button* installCoreX11
          { installCoreOnly = new Fl_Round_Button(138, 95, 238, 20, mygettext("Core Only (Text Based Interface)"));
            installCoreOnly->type(102);
            installCoreOnly->down_box(FL_ROUND_DOWN_BOX);
          } // Fl_Round_Button* installCoreOnly
          installType->end();
        } // Fl_Group* installType
        { corePlusExtensions = new Fl_Group(83, 155, 315, 180, mygettext("Core Plus Extensions Catagories to Install"));
          { Fl_Box* o = new Fl_Box(101, 160, 290, 80, mygettext("Wireless "));
            o->box(FL_DOWN_BOX);
            o->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));
          } // Fl_Box* o
          { btnWifi = new Fl_Check_Button(151, 174, 238, 25, mygettext("Wifi Support"));
            btnWifi->down_box(FL_DOWN_BOX);
          } // Fl_Check_Button* btnWifi
          { btnNdiswrapper = new Fl_Check_Button(151, 194, 238, 25, mygettext("ndiswrapper"));
            btnNdiswrapper->down_box(FL_DOWN_BOX);
          } // Fl_Check_Button* btnNdiswrapper
          { btnWifiFirmware = new Fl_Check_Button(151, 214, 238, 25, mygettext("Wireless Firmware"));
            btnWifiFirmware->down_box(FL_DOWN_BOX);
          } // Fl_Check_Button* btnWifiFirmware
          { Fl_Box* o = new Fl_Box(101, 245, 290, 80, mygettext("Other"));
            o->box(FL_DOWN_FRAME);
            o->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));
          } // Fl_Box* o
          { btnInstaller = new Fl_Check_Button(151, 258, 238, 25, mygettext("Installer Application"));
            btnInstaller->down_box(FL_DOWN_BOX);
          } // Fl_Check_Button* btnInstaller
          { btnRemaster = new Fl_Check_Button(151, 278, 238, 25, mygettext("Remaster Tool"));
            btnRemaster->down_box(FL_DOWN_BOX);
          } // Fl_Check_Button* btnRemaster
          { btnKmaps = new Fl_Check_Button(151, 298, 238, 25, mygettext("Non-US keyboard layout support"));
            btnKmaps->down_box(FL_DOWN_BOX);
          } // Fl_Check_Button* btnKmaps
          corePlusExtensions->end();
        } // Fl_Group* corePlusExtensions
        grpExtensionsCorePlus->end();
      } // Fl_Group* grpExtensionsCorePlus
      { grpExtensionsStandAlone = new Fl_Group(36, 61, 405, 255);
        grpExtensionsStandAlone->hide();
        { Fl_Group* o = new Fl_Group(52, 111, 389, 205, mygettext("Extension Installation"));
          { btnInstallExtensions = new Fl_Round_Button(54, 136, 382, 25, mygettext("Install Extensions from this TCE/CDE Directory:"));
            btnInstallExtensions->type(102);
            btnInstallExtensions->down_box(FL_ROUND_DOWN_BOX);
            btnInstallExtensions->callback((Fl_Callback*)cb_btnInstallExtensions);
            btnInstallExtensions->value(1);
          } // Fl_Round_Button* btnInstallExtensions
          { btnNoExtensions = new Fl_Round_Button(54, 215, 382, 25, mygettext("Don\'t install Extensions (Core Only, Text Based Install)"));
            btnNoExtensions->type(102);
            btnNoExtensions->down_box(FL_ROUND_DOWN_BOX);
            btnNoExtensions->callback((Fl_Callback*)cb_btnNoExtensions);
          } // Fl_Round_Button* btnNoExtensions
          { outputTCEDir = new Fl_Output(66, 159, 375, 27);
            outputTCEDir->callback((Fl_Callback*)cb_outputTCEDir, (void*)("tce"));
          } // Fl_Output* outputTCEDir
          o->end();
        } // Fl_Group* o
        grpExtensionsStandAlone->end();
      } // Fl_Group* grpExtensionsStandAlone
      { grpReview = new Fl_Group(25, 30, 435, 325);
        grpReview->hide();
        { brwReview = new Fl_Browser(45, 55, 390, 245, mygettext("Review"));
          brwReview->align(Fl_Align(FL_ALIGN_TOP));
        } // Fl_Browser* brwReview
        { btnProceed = new Fl_Button(210, 315, 64, 20, mygettext("Proceed"));
          btnProceed->callback((Fl_Callback*)btnProceedCB);
        } // Fl_Button* btnProceed
        grpReview->end();
      } // Fl_Group* grpReview
      wWizard->value(wWizard->child(0));
      wWizard->end();
    } // Fl_Wizard* wWizard
    { grpButtons = new Fl_Group(25, 365, 440, 25);
      grpButtons->deactivate();
      { Fl_Button* o = new Fl_Button(180, 365, 45, 25, mygettext("@<"));
        o->callback((Fl_Callback*)btnCB, (void*)("prev"));
      } // Fl_Button* o
      { Fl_Button* o = new Fl_Button(230, 365, 45, 25, mygettext("@>"));
        o->callback((Fl_Callback*)btnCB, (void*)("next"));
      } // Fl_Button* o
      grpButtons->end();
    } // Fl_Group* grpButtons
    window->end();
    window->resizable(window);
  } // Fl_Double_Window* window
  test4syslinux = system("which syslinux >/dev/null");
  if ( test4syslinux != 0 )
  {
     fl_message("tc-install requires syslinux extension to be loaded.\nHDD (vfat) & ZIP installs require dosfstools.\nZIP will also require perl.");
     exit(1);
  }
  
  
  int cd_num;
  char command_buff[36];
  char path_buff[29];
  char tce_path_buff[13];
  for (cd_num = 0; cd_num < 10; cd_num++) {
    sprintf(command_buff, "[ -f /mnt/sr%d/boot/core.gz ]", cd_num);
    results = system(command_buff);
    if (results == 0){
      sprintf(path_buff, "/mnt/sr%d/boot/core.gz", cd_num);
      fullpathOutput->value(path_buff);
      path = path_buff;
      image = "core";
      break;
    } else {
      sprintf(command_buff, "[ -f /mnt/sr%d/boot/corepure64.gz ]", cd_num);
      results = system(command_buff);
      if (results == 0){
        sprintf(path_buff, "/mnt/sr%d/boot/corepure64.gz", cd_num);
        fullpathOutput->value(path_buff);
        path = path_buff;
        image = "corepure64";
        break;
      }
    }
  }
  
  if (cd_num != 10) {
    sprintf(command_buff, "[ -d /mnt/sr%d/cde ]", cd_num);
    results = system(command_buff);
    if (results == 0){
      sprintf(tce_path_buff, "/mnt/sr%d/cde", cd_num);
      outputTCEDir->value(tce_path_buff);
      tcepath = tce_path_buff;
    }
  }
  
  brwTarget->deactivate();
  prepTarget();
  window->show(argc, argv);
  return Fl::run();
}
