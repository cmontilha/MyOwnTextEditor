#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <cstring>
#include <cerrno>

// Global variables
int changed = 0;
char filename[256] = "";
Fl_Text_Buffer *textbuf;

// Function to save a file
void save_file(char *newfile) {
    if (textbuf->savefile(newfile)) {
        fl_alert("Error writing to file: %s", strerror(errno));
    } else {
        strcpy(filename, newfile);
        changed = 0;
        textbuf->call_modify_callbacks();
    }
}

// Function to load a file
void load_file(char *newfile) {
    if (textbuf->loadfile(newfile)) {
        fl_alert("Error reading file: %s", strerror(errno));
    } else {
        strcpy(filename, newfile);
        changed = 0;
        textbuf->call_modify_callbacks();
    }
}

// Check if the file needs to be saved before other operations
int check_save() {
    if (!changed) return 1;
    int r = fl_choice("The current file has not been saved. Save it now?", "Cancel", "Save", "Discard");
    if (r == 1) {
        save_file(filename);
        return !changed;
    }
    return (r == 2) ? 1 : 0;
}

// Callback for "New File"
void new_cb(Fl_Widget*, void*) {
    if (!check_save()) return;
    filename[0] = '\0';
    textbuf->text("");
    changed = 0;
}

// Callback for "Open File"
void open_cb(Fl_Widget*, void*) {
    if (!check_save()) return;
    char *newfile = fl_file_chooser("Open File?", "*", filename);
    if (newfile != NULL) load_file(newfile);
}

// Callback for "Save File"
void save_cb(Fl_Widget*, void*) {
    if (filename[0] == '\0') {
        char *newfile = fl_file_chooser("Save File As?", "*", filename);
        if (newfile != NULL) save_file(newfile);
    } else {
        save_file(filename);
    }
}

// Callback for "Save As"
void saveas_cb(Fl_Widget*, void*) {
    char *newfile = fl_file_chooser("Save File As?", "*", filename);
    if (newfile != NULL) save_file(newfile);
}

// Callback for "Exit"
void quit_cb(Fl_Widget*, void*) {
    if (changed && !check_save()) return;
    exit(0);
}

// Function to create a new window with the editor and buttons at the top
Fl_Window* new_editor() {
    Fl_Window *win = new Fl_Window(800, 600, "Simple Text Editor");

    // Buttons at the top of the window
    Fl_Button *new_button = new Fl_Button(5, 5, 80, 30, "New");
    new_button->callback(new_cb);
    new_button->color(FL_DARK_GREEN);  // Dark green button

    Fl_Button *open_button = new Fl_Button(90, 5, 80, 30, "Open");
    open_button->callback(open_cb);
    open_button->color(FL_DARK_GREEN);  // Dark green button

    Fl_Button *save_button = new Fl_Button(175, 5, 80, 30, "Save");
    save_button->callback(save_cb);
    save_button->color(FL_DARK_GREEN);  // Dark green button

    Fl_Button *saveas_button = new Fl_Button(260, 5, 80, 30, "Save As");
    saveas_button->callback(saveas_cb);
    saveas_button->color(FL_DARK_GREEN);  // Dark green button

    Fl_Button *exit_button = new Fl_Button(345, 5, 80, 30, "Exit");
    exit_button->callback(quit_cb);
    exit_button->color(FL_DARK_GREEN);  // Dark green button

    // Text editor below the buttons
    Fl_Text_Editor *editor = new Fl_Text_Editor(0, 40, 800, 560);
    editor->buffer(textbuf);
    editor->textfont(FL_COURIER);

    win->end();
    return win;
}

int main(int argc, char **argv) {
    textbuf = new Fl_Text_Buffer();
    textbuf->add_modify_callback([](int, int nInserted, int nDeleted, int, const char*, void*) {
        if (nInserted || nDeleted) changed = 1;
    }, nullptr);

    Fl_Window *window = new_editor();
    window->show(argc, argv);
    return Fl::run();
}
