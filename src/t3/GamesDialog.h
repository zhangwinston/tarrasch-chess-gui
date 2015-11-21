/****************************************************************************
 * Custom dialog - list of games from database, file, session, or clipboard
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMES_DIALOG_H
#define GAMES_DIALOG_H
#include <unordered_set>

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/accel.h"
#include "SuspendEngine.h"
#include "GamesCache.h"
#include "GameDocument.h"
#include "Repository.h"
#include "MiniBoard.h"
#include "Database.h"

// Control identifiers
enum
{
    ID_DB_CHECKBOX  = 10000,
    ID_DB_CHECKBOX2,
    ID_DB_RADIO ,
    ID_DB_COMBO ,
    ID_DB_RELOAD,
    ID_DB_UTILITY,
    ID_DB_TEXT   ,
    ID_DB_LISTBOX_GAMES,
    ID_DB_LISTBOX_STATS,
    ID_DB_LISTBOX_TRANSPO,
    ID_BUTTON_1,
    ID_BUTTON_2,
    ID_BUTTON_3,
    ID_BUTTON_4,
    ID_PGN_DIALOG_FILE     ,
    ID_PGN_DIALOG_CLIPBOARD,
    ID_PGN_DIALOG_SESSION  ,
    ID_BOARD2GAME          ,
    ID_PGN_DIALOG_GAME_DETAILS,
    ID_REORDER           ,
    ID_ADD_TO_CLIPBOARD  ,
    ID_SAVE_ALL_TO_A_FILE,
    ID_PGN_DIALOG_GAME_PREFIX,
    ID_PGN_DIALOG_PUBLISH    ,
    ID_PGN_DIALOG_DATABASE   ,
    ID_PGN_DIALOG_UTILITY1   ,
    ID_PGN_DIALOG_UTILITY2   
};

// Track the game presented on the mini board
struct MiniBoardGame
{
    thc::ChessPosition  updated_position;
    CompactGame         info;
    int                 focus_idx;
    int                 focus_offset;
};


// A GamesListCtrl is a list control within the GamesDialog, it displays a list of games to navigate through and pick from
class GamesDialog;
class GamesListCtrl: public wxListCtrl
{
    DECLARE_EVENT_TABLE()
public:
    GamesListCtrl( GamesDialog *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style );
    void OnChar( wxKeyEvent &event );
    
public:
    GamesDialog   *parent;
    int initial_focus_offset;
    MiniBoard *mini_board;
    MiniBoardGame *track;
    
    // Focus changes to new item;
    void ReceiveFocus( int focus_idx );
    
    // Recipes to calculate move text
    std::string CalculateMoveTxt() const;
    std::string CalculateMoveTxt( std::string &previous_move ) const;
    std::string CalculateMoveTxt( CompactGame &info, int offset ) const;
    std::string CalculateMoveTxt( std::string &previous_move, CompactGame &info, int focus_offset, thc::ChessPosition &updated_position ) const;
    
protected:
    virtual wxString OnGetItemText( long item, long column) const;
};


// GamesDialog class declaration
class GamesDialog: public wxDialog
{    
    DECLARE_CLASS( GamesDialog )
    DECLARE_EVENT_TABLE()

protected:
    thc::ChessRules cr;
    thc::ChessRules cr_base;
    wxStaticText *title_ctrl;
 
public:

    // Track the game presented on the mini board
    MiniBoardGame mini_board_game;
    MiniBoardGame *track;

    // Constructors
    GamesDialog
    (
        wxWindow    *parent,
        thc::ChessRules *cr,
        GamesCache  *gc,
        GamesCache  *gc_clipboard,
        wxWindowID  id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
    );
    
    // Member initialisation
    void Init();

    // Creation
    bool Create
    (
        wxWindow        *parent,
        wxWindowID      id,
        const wxString& caption = wxT("Pgn browser"),
        const wxPoint&  pos = wxDefaultPosition,
        const wxSize&   size = wxDefaultSize,
        long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
    );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // GamesDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );
    void OnActivate(wxActivateEvent& event);
    void OnListSelected( wxListEvent &event );
    void OnListFocused( wxListEvent &event );
    void OnListColClick( wxListEvent &event );
    void OnTabSelected( wxBookCtrlEvent &event );
    void OnNextMove( wxCommandEvent &event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnBoard2Game( wxCommandEvent& event );
    void OnRenumber( wxCommandEvent& event );
    void OnSelectAll( wxCommandEvent& event );
    void OnEditGameDetails( wxCommandEvent& event );
    void OnEditGamePrefix( wxCommandEvent& event );
    void OnCopy( wxCommandEvent& event );
    void OnAddToClipboard( wxCommandEvent& event );
    void OnSaveAllToAFile( wxCommandEvent& event );
    void OnCut( wxCommandEvent& event );
    void OnDelete( wxCommandEvent& event );
    void OnPaste( wxCommandEvent& event );
    void OnSave( wxCommandEvent& event );
    void OnPublish( wxCommandEvent& event );
    void OnHelpClick( wxCommandEvent& event );
    
    void OnSearch( wxCommandEvent& event );
    void OnUtility( wxCommandEvent& event );
    void OnButton1( wxCommandEvent& event );
    void OnButton2( wxCommandEvent& event );
    void OnButton3( wxCommandEvent& event );
    void OnButton4( wxCommandEvent& event );
    void OnRadio( wxCommandEvent& event );
    void OnSpin( wxCommandEvent& event );
    void OnComboBox( wxCommandEvent& event );
    void OnCheckBox( wxCommandEvent& event );
    void OnCheckBox2( wxCommandEvent& event );
    void OnListSelected( int idx );

    void Goto( int idx );
    void ReadItemWithSingleLineCache( int item, CompactGame &info );

    // Overrides - Gdv = Games Dialog Override
    virtual void GdvOnActivate();
    virtual wxSizer *GdvAddExtraControls() { return NULL; }
    virtual void GdvGetButtonGridDimensions( int &row1, int &col1, int &row2, int &col2 ) { row1=8; col1=2; row2=0; col2=0; }
    bool dirty;
    virtual bool GdvTestAndClearIsCacheDirty() { bool was=dirty; dirty=false; return was; }
    virtual void GdvReadItem( int item, CompactGame &info ) = 0;
    virtual void GdvOnCancel();
    virtual void GdvListColClick( int compare_col );
    virtual void GdvSaveAllToAFile();
    virtual void GdvHelpClick();
    virtual void GdvCheckBox( bool checked );
    virtual void GdvCheckBox2( bool checked );
    virtual void GdvSearch();
    virtual void GdvUtility();
    virtual void GdvButton1();
    virtual void GdvButton2();
    virtual void GdvButton3();
    virtual void GdvButton4();
    virtual void GdvNextMove( int idx );

    // Todo later
    void OnEditGameDetails( wxCommandEvent );
    void OnEditGamePrefix( wxCommandEvent );
    //void OnSaveAllToAFile();
    void OnAddToClipboard( wxCommandEvent );
    void OnCopy( wxCommandEvent );
    //void CopyOrAdd( bool clear_clipboard );
    void OnCut( wxCommandEvent );
    void OnDelete( wxCommandEvent );
    void OnPaste( wxCommandEvent );
    void OnSave( wxCommandEvent );
    void OnPublish( wxCommandEvent );

    
//  void OnClose( wxCloseEvent& event );
//  void SaveColumns();
    bool ShowModalOk( std::string title );
    
    // Return true if a game has been selected
    bool LoadGame(  GameDocument &gd );
    void LoadGame( int idx, int focus_offset=0 );
    
 
    // Helpers
    GameDocument *GetFocusGame( int &idx );
    void DeselectOthers();
    void OnOk();
    void SmartCompare( std::vector< smart_ptr<ListableGame> > &gds );
    
    // GamesDialog member variables
public:
    wxStaticText *player_names;
    CompactGame   single_line_cache;
    int           single_line_cache_idx;
    int           compare_col;
    
protected:
    wxWindow* parent2;
    wxWindowID id;
    wxPoint pos;
    wxSize size;
    long style;
    DB_REQ db_req;

    GamesListCtrl  *list_ctrl;
    wxBoxSizer*  hsiz_panel;
    wxBoxSizer *button_panel;
    wxFlexGridSizer* vsiz_panel_button1;
    wxFlexGridSizer* vsiz_panel_buttons;
    
    wxNotebook  *notebook;
    int          selected_game;
    void         SyncCacheOrderBefore();
    void         SyncCacheOrderAfter();
    void         CopyOrAdd( bool clear_clipboard );

    // Data members
    wxButton* ok_button;
    wxCheckBox *filter_ctrl;
    wxCheckBox *white_player_ctrl;
    wxRadioButton *radio_ctrl;
    wxComboBox *combo_ctrl;
    wxTextCtrl *text_ctrl;
    wxListBox *list_ctrl_stats;
    wxListBox *list_ctrl_transpo;
    wxButton *utility;
    MiniBoard *mini_board;
    bool activated_at_least_once;

private:    //TODO - move more vars to private
    bool db_search;
public:
    bool transpo_activated;
    int nbr_games_in_list_ctrl;
    
protected:
    int file_game_idx;
    bool db_game_set;
    GamesCache  *gc;
    GamesCache  *gc_clipboard;
    GameDocument db_game;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
};


#endif    // GAMES_DIALOG_H
