/****************************************************************************
 * Games Cache - Abstracted interface to  a list of games
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMES_CACHE_H
#define GAMES_CACHE_H
#include <time.h> // time_t
#include "GameDocument.h"
#include "PackedGame.h"
#include "CompressMoves.h"

class DB_GAME_INFO : public MagicBase
{
public:
    virtual DB_GAME_INFO  *GetDbGameInfoPtr()  { return this; }
	virtual GameDocument  *GetGameDocumentPtr()
    {
        static GameDocument  the_game;
        CompactGame pact;
        GetCompactGame( pact );
        pact.Upscale(the_game);
        return &the_game;
    }

    int game_id;
    Roster r;
    std::string str_blob;
    
    std::string Description();
    virtual bool HaveStartPosition() { return false; }
    virtual thc::ChessPosition &GetStartPosition() { cprintf("FIXME (MAYBE) DANGER WILL ROBINSON 6\n"); static thc::ChessPosition start; return start; }
    virtual Roster &RefRoster() { return r; }
    virtual std::vector<thc::Move> &RefMoves()
    {
        static std::vector<thc::Move> moves;
        CompressMoves press;
        moves = press.Uncompress(str_blob);
        return moves;
    }
    virtual std::string &RefCompressedMoves()
    {
        return str_blob;
    }
    virtual thc::ChessPosition &RefStartPosition() { static thc::ChessPosition start; return start;  }
};

void ReadGameFromPgn( int pgn_handle, long fposn, GameDocument &gd );

class PgnDocument : public MagicBase
{
private:
    int  pgn_handle;
    long fposn;
    //bool in_memory;
    //Roster roster;
    //std::string blob;
    PackedGame pack;
public:
    PgnDocument( int pgn_handle, long fposn ) { this->pgn_handle=pgn_handle, this->fposn = fposn; /*in_memory=false;*/ }

    virtual void GetCompactGame( CompactGame &pact )
    {
        if( !pack.Empty() )
            pack.Unpack(pact);
        else
        {
            GameDocument the_game;
            ReadGameFromPgn( pgn_handle, fposn, the_game );
            pact.Downscale( the_game );
            pack.Pack(pact);
        }
    }
    
	virtual GameDocument  *GetGameDocumentPtr()
    {
        static GameDocument  the_game;
        cprintf("FIXME DANGER WILL ROBINSON (ptr to static 1)\n");
        ReadGameFromPgn( pgn_handle, fposn, the_game );
        return &the_game;
    }
    virtual long GetFposn() { return fposn; }
    virtual Roster &RefRoster()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.r;
    }
    virtual std::vector<thc::Move> &RefMoves()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.moves;
    }
    virtual std::string &RefCompressedMoves()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        CompressMoves press( pact.start_position );
        static std::string blob;
        blob.clear();
        blob = press.Compress( pact.moves );
        return blob;
    }
    virtual thc::ChessPosition &RefStartPosition()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.start_position;
    }
    
};


class GamesCache
{    
public:
    std::vector< smart_ptr<MagicBase> >  gds;
    std::vector<int>           col_flags;
    std::string                pgn_filename;
    int game_nbr;
    bool renumber;
    bool file_irrevocably_modified;

    GamesCache() { state=PREFIX; renumber=false; resume_previous_window=false; loaded=false; top_item=0;
                    file_irrevocably_modified=false; }
    void Debug( const char *intro_message );
    bool Load( std::string &filename );
    bool Reload() { return Load(pgn_filename); }
    bool Load( FILE *pgn_file );
    bool FileCreate( std::string &filename, GameDocument &gd );
    void FileSave( GamesCache *gc_clipboard );
    void FileSaveAs( std::string &filename, GamesCache *gc_clipboard );
    void FileSaveGameAs( std::string &filename, GamesCache *gc_clipboard );
    void FileSaveAllAsAFile( std::string &filename );
    void FileSaveInner( GamesCache *gc_clipboard, FILE *pgn_in, FILE *pgn_out );
    void Publish( GamesCache *gc_clipboard );
    bool IsLoaded();
    bool IsSynced();
    void KillResumePreviousWindow()
    {
        resume_previous_window=false;
    }
    void PrepareForResumePreviousWindow( int top_item )
    {
        resume_previous_window=true; this->top_item = top_item;
    }
    bool IsResumingPreviousWindow( int &top_item )
    {
        top_item=this->top_item; return resume_previous_window; 
    }

    // Helpers
    int  pgn_handle;
private:
    enum {PREFIX,HEADER,INGAME} state;
    bool resume_previous_window;
    int  top_item;
    bool loaded;

    // Check whether text s is a valid header, return true if it is,
    //  add info to a GameDocument, optionally clearing it first
public:
    bool Tagline( GameDocument &gd,  const char *s );
};

#endif    // GAMES_CACHE_H
