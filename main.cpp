#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/choice.h>
#include <wx/process.h>
#include <vector>
#include <cstdio>

class FlashFrame : public wxFrame {
public:
    FlashFrame() : wxFrame(nullptr, wxID_ANY, "iFlash", wxDefaultPosition, wxSize(500, 300)) {
        auto* panel = new wxPanel(this);
        auto* vbox = new wxBoxSizer(wxVERTICAL);

        // Drive selection
        vbox->Add(new wxStaticText(panel, wxID_ANY, "Select Drive:"), 0, wxALL, 5);
        driveChoice = new wxChoice(panel, wxID_ANY);
        vbox->Add(driveChoice, 0, wxEXPAND | wxALL, 5);

        // Image selection
        vbox->Add(new wxStaticText(panel, wxID_ANY, "Select Image (IMG/ISO):"), 0, wxALL, 5);
        imagePicker = new wxFilePickerCtrl(
            panel,
            wxID_ANY,
            "",
            "Select image",
            "*.img;*.iso",
            wxDefaultPosition,
            wxDefaultSize,
            wxFLP_OPEN | wxFLP_FILE_MUST_EXIST
        );
        vbox->Add(imagePicker, 0, wxEXPAND | wxALL, 5);

        // Flash button
        flashButton = new wxButton(panel, wxID_ANY, "Flash!");
        vbox->Add(flashButton, 0, wxALIGN_CENTER | wxALL, 10);

        // Log output
        log = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                             wxTE_MULTILINE | wxTE_READONLY);
        vbox->Add(log, 1, wxEXPAND | wxALL, 5);

        panel->SetSizer(vbox);

        PopulateDrives();

        flashButton->Bind(wxEVT_BUTTON, &FlashFrame::OnFlash, this);
    }

private:
    wxChoice* driveChoice;
    wxFilePickerCtrl* imagePicker;
    wxButton* flashButton;
    wxTextCtrl* log;

    void PopulateDrives() {
        driveChoice->Clear();

#if defined(__linux__)
        FILE* fp = popen("lsblk -dpno NAME,SIZE | grep -E '/dev/sd|/dev/mmc|/dev/disk'", "r");
        if (!fp) return;

        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            driveChoice->Append(wxString::FromUTF8(line).Trim());
        }
        pclose(fp);
#elif defined(__APPLE__)
        FILE* fp = popen("diskutil list | grep '^/dev/'", "r");
        if (!fp) return;

        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            wxString s(line);
            s.Trim(true).Trim(false); // trim leading and trailing whitespace
            driveChoice->Append(s);
        }
        pclose(fp);
#endif
    }

    void OnFlash(wxCommandEvent&) {
        if (driveChoice->GetSelection() == wxNOT_FOUND) {
            wxMessageBox("Please select a drive.", "Error", wxICON_ERROR);
            return;
        }

        if (imagePicker->GetPath().IsEmpty()) {
            wxMessageBox("Please select an image file.", "Error", wxICON_ERROR);
            return;
        }

        wxString driveLine = driveChoice->GetStringSelection();
        wxString drive = driveLine.BeforeFirst(' ');
        wxString image = imagePicker->GetPath();

        int confirm = wxMessageBox(
            "THIS WILL ERASE ALL DATA ON:\n\n" + drive + "\n\nContinue?",
            "Confirm Flash",
            wxYES_NO | wxICON_WARNING
        );

        if (confirm != wxYES) return;

        log->AppendText("Flashing started...\n");

        wxString cmd;
#if defined(__APPLE__)
        cmd = wxString::Format(
            "sudo dd if='%s' of='%s' bs=4m status=progress",
            image,
            drive
        );
#else
        cmd = wxString::Format(
            "sudo dd if='%s' of='%s' bs=4M status=progress conv=fsync",
            image,
            drive
        );
#endif

        log->AppendText("Running command:\n" + cmd + "\n");

        wxExecute(cmd, wxEXEC_ASYNC);
    }
}; // <-- FlashFrame properly closed

// FlashApp at global scope
class FlashApp : public wxApp {
public:
    bool OnInit() override {
        auto* frame = new FlashFrame();
        frame->Show();
        return true;
    }
};

// wxIMPLEMENT_APP at global scope
wxIMPLEMENT_APP(FlashApp);
