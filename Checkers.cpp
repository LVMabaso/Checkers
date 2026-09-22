#include <raylib.h>
//#include <iostream>
#include "CheckersBot\CheckerTools.hpp"
#include <thread>
#include <atomic>
#include <iterator> 
using namespace CheckTools;

    const int CLIENT_WIDTH = 1600;
    const int CLIENT_HEIGHT = 900;
    const Color BACKGROUND_COLOR = {0,255,255,255};
    const Color DARK_SQUARE_COLOR = {115, 149, 82,255};
    const Color LIGHT_SQUARE_COLOR = {235, 236, 208,255};
    const int SQUARE_WIDTH = 100;
    const int X_BOARD_OFFSET = 300;
    const int Y_BOARD_OFFSET = 90;
    const float PIECE_SIZE = 0.8;
    const float PIECE_OUTLINE_SIZE = 0.4;
    const Color BLACK_COLOR = {0,0,0,255};
    const Color WHITE_COLOR = {255,255,255,255};
    const Color BLACK_OUTLINE = {32,32,32,255};
    const Color WHITE_OUTLINE = {244,224,224,255};
    const Color KING_COLOR = {255,215,0,255};
    const Color HIGHLIGHT_COLOR = {185, 202, 67,255};
    const Color HIGHLIGHT2_COLOR = {0,0,0,32};
    const float HIGHLIGHT2_SIZE = 0.5;
    const string STARTING_POSITION = "W:Wa3,c3,e3,g3,b2,d2,f2,h2,a1,c1,e1,g1:Bb8,d8,f8,h8,a7,c7,e7,g7,b6,d6,f6,h6";
    
std::atomic<bool> inMotion;
std::atomic<Bitboard> BB;
std::atomic<bool> playersTurn = true;
atomic<double> eval = 0;


vector<vector <Move>> MoveList(64);
Move currentMove;
int cores = std::thread::hardware_concurrency();
int Gdepth = 12;

Color invert(Color col)
{
    return {255 - col.r,255 - col.g,255 - col.b,col.a};
}

class button
{
    private: 
     string text;
     int fontSize;
     Vector2 pos;
     Vector2 size;
     Color col;
    public:
    void (*function)();
    button(string txt,int FS,Vector2 position,Vector2 Size,Color color)
    {
        text = txt;
        fontSize = FS;
        pos = position;
        size = Size;
        col = color;
        
    }
    
    button(){};
    
    void setText(string txt)
    {
        text = txt;
    }
    
    void setFontSize(int FS)
    {
        fontSize = FS;
    }
    
    void setPos(Vector2 position)
    {
        pos = position;
    }
    
    void setSize(Vector2 Size)
    {
        size = Size;
    }
    
    void setColor(Color color)
    {
        col = color;
    }
    
    void setFunction(void (*func)() )
    {
        function = func;
    }
    
    string getText()
    {
       return text;
    }
    
    int getFontSize()
    {
       return fontSize;
    }
    
    Vector2 getPos()
    {
       return pos;
    }
    
    Vector2 getSize()
    {
       return size;
    }
    
    Color getColor()
    {
       return col;
    }
    
    bool inBounds(Vector2 cords)
    {
        return (cords.x > pos.x) && (cords.x < pos.x+size.x) && (cords.y > pos.y) && (cords.y < pos.y +size.y);
    }
    
    void Draw()
    {
        DrawRectangle(pos.x,pos.y,size.x,size.y,col);
        DrawText(text.c_str(),pos.x+size.x/10,pos.y+size.y/2,fontSize,invert(col));
    }
    
    
    
};


void Bot(int depth, int cores)
{
    cout<<"Bot Initialized"<<endl;
    
   while (true){
  if (!inMotion.load()){continue;}
   evalMove mve = bestMove(BB.load(),depth,cores);
   currentMove = mve.mv;
   eval = mve.eval;
   inMotion = false;
   }
}

void InitializeMoveList()
{
    vector <Move> MList;
    getMoves(BB,&MList);
    MoveList.clear();
    for (int i = 0;i<64;i++){MoveList[i].clear();MoveList[i].shrink_to_fit();}
    for (size_t i = 0;i<MList.size();i++)
    {
        char firstSQ = MList[i].captureChain[0];
        MoveList[firstSQ].push_back(MList[i]);
    }
}

void MoveHandler(Move* mv)
{
    BB = Makemove(BB,mv);
    InitializeMoveList();
    (*mv).captureChain.clear();
   if (BB.load().bturn != playersTurn.load() && !inMotion.load() )
    {
        inMotion = true;
        //std::thread BotThread(Bot,Gdepth,cores);
    }
  
    
}





void HighlightSq(char sq)
{
    char i = sq%8;
    char j = floor(sq/8);
    j = 7 - j;
    if (inrange(sq))
    {
        
       Rectangle rec = {X_BOARD_OFFSET + i*SQUARE_WIDTH,Y_BOARD_OFFSET + j*SQUARE_WIDTH,SQUARE_WIDTH,SQUARE_WIDTH};
       DrawRectangleLinesEx(rec, 5.0f,HIGHLIGHT_COLOR);
    }
}

void HighlightSq2(int sq)
{
    int i = sq%8;
    int j = floor(sq/8);
    j = 7 - j;
    if (inrange(sq))
    {
       DrawCircle(X_BOARD_OFFSET + (i+0.5)*SQUARE_WIDTH,Y_BOARD_OFFSET + (j+0.5)*SQUARE_WIDTH,SQUARE_WIDTH*HIGHLIGHT2_SIZE/2,HIGHLIGHT2_COLOR);
    }    
}

void HighlightPlayableMoves(char sq)
{
    
    if (sq == -1){return;}
    for (size_t i = 0;i<MoveList[sq].size();i++)
    {
        for (size_t j = 1;j<MoveList[sq][i].captureChain.size();j++ ) 
        {
            int highlightee = MoveList[sq][i].captureChain[j];
            HighlightSq2(highlightee);
        }
    }
}

void PlacePiece(char square,bool color,bool isKing)
    {
        int i = square%8;
        int j = floor(square/8);
        DrawCircle(X_BOARD_OFFSET + i*SQUARE_WIDTH+SQUARE_WIDTH/2 , Y_BOARD_OFFSET + j*SQUARE_WIDTH+SQUARE_WIDTH/2 , SQUARE_WIDTH *PIECE_SIZE/2 , color?WHITE_COLOR:BLACK_COLOR);
        
        DrawCircle(X_BOARD_OFFSET + i*SQUARE_WIDTH +SQUARE_WIDTH/2 , Y_BOARD_OFFSET + j*SQUARE_WIDTH + SQUARE_WIDTH/2 , SQUARE_WIDTH * PIECE_SIZE * PIECE_OUTLINE_SIZE/2 , color?WHITE_OUTLINE:BLACK_OUTLINE);
        
        if (isKing){DrawCircle(X_BOARD_OFFSET + i*SQUARE_WIDTH + SQUARE_WIDTH/2 , Y_BOARD_OFFSET + j*SQUARE_WIDTH + SQUARE_WIDTH/2 , SQUARE_WIDTH*PIECE_SIZE*PIECE_OUTLINE_SIZE/2 , KING_COLOR);}
    }
void DrawBoard()
    {
        for (int i = 0;i<8;i++)
            {
                for (int j = 0;j<8;j++)
                    {
                    if ( (j+i)%2)
                    {        
                    DrawRectangle(X_BOARD_OFFSET + i*SQUARE_WIDTH , Y_BOARD_OFFSET + j*SQUARE_WIDTH , SQUARE_WIDTH, SQUARE_WIDTH,DARK_SQUARE_COLOR );
                    } 
                    else
                    {
                      DrawRectangle(X_BOARD_OFFSET + i*SQUARE_WIDTH , Y_BOARD_OFFSET + j*SQUARE_WIDTH ,SQUARE_WIDTH , SQUARE_WIDTH ,LIGHT_SQUARE_COLOR );  
                    }       
                            
                    }
            }
    }
void DrawPieces(Bitboard BBoard)
{
    
for (int i = 0;i<64;i++)
    { 

    bool BBBuffer[] = {0,0,0,0};
    for (int j = 0;j < 4; j++)
        {
         BBBuffer[j] = getbit(BBoard.BB[j],i);
        }
     if (BBBuffer[B])
        {
        PlacePiece( (7 - floor(i/8))*8 + i%8,B,BBBuffer[3]);
        }
     else if (BBBuffer[W])
        {
                    PlacePiece( (7 - floor(i/8))*8 + i%8 ,W,BBBuffer[3]);
        }
        

    }
}

int getSquare(Vector2 vec)
    {
        if ( (vec.x < X_BOARD_OFFSET) || (vec.x>X_BOARD_OFFSET+8*SQUARE_WIDTH) || (vec.y < Y_BOARD_OFFSET) || (vec.y>Y_BOARD_OFFSET+8*SQUARE_WIDTH)  )
        {
            return -1;
        }
        int i,j;
        i = (vec.x - X_BOARD_OFFSET)/SQUARE_WIDTH;
        j = (int)((vec.y - Y_BOARD_OFFSET)/SQUARE_WIDTH);
        
        return (7 - j)*8 + i;
    }


void PrintMoveList()
{
    for (int i = 0;i<64;i++)
    {
        cout<<"Move on sq "<<i<<endl;
        for (size_t j = 0;j<MoveList[i].size();j++)
        {
            cout<<"Move no "<<j<<endl;
            for (size_t k = 0;k<MoveList[i][j].captureChain.size();k++)
            {
                cout<<(int)MoveList[i][j].captureChain[k]<<" ";
            }
            cout<<endl;
        }
    }
}


//Button functions----------------------------------------------
void resetBoard()
{
    BB = FENtoboard(STARTING_POSITION);
    InitializeMoveList();
}

//--------------------------------------------------------------

int main() 
{
    
   InitWindow(CLIENT_WIDTH,CLIENT_HEIGHT,"checkers");
    SetTargetFPS(60);
inMotion = playersTurn.load() == BB.load().bturn;
   
int sq = -1; 
BB = FENtoboard(STARTING_POSITION);
//BB = FENtoboard(STARTING_POSITION);
InitializeMoveList();
PrintMoveList();
printBB(BB);
std::thread BotThread(Bot,Gdepth,cores);

vector<button> btns(1);
Vector2 v = {100,100};

btns[0] = button("Reset Board",12,{20,100},{200,50},BLACK);

btns[0].function = resetBoard;
   while (!WindowShouldClose())
    {
       DrawText( to_string(eval.load()).c_str(),300,50,12,{255,255,255,255});
        
       if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) )
       {
           Vector2 pos = GetMousePosition();
           //----------checking button clicks
           for (int i = 0;i<btns.size();i++)
           {
               if (btns[i].inBounds(pos))
               {
                   btns[i].function();
                   break;
               }
           }
          //------------------------
       
        if (playersTurn.load() == BB.load().bturn)
        {       
           int clickedSq = getSquare(pos);
           if (inrange(clickedSq))
           {
               if (sq == -1)  
               {
                   sq = clickedSq;
               }
               else 
               {
                   for (size_t i = 0;i<MoveList[sq].size();i++)
                   {
                       size_t lastElement = MoveList[sq][i].captureChain.size() - 1;//size_t ooooh fancy
                           if (MoveList[sq][i].captureChain[0] == sq && MoveList[sq][i].captureChain[lastElement] == clickedSq)
                           {
                               currentMove = MoveList[sq][i];
                               printMove(&currentMove);
                           }
                       
                   }
                   sq = -1;
                   
               }
           }
       }
       }
        
      
        
        if (currentMove.captureChain.size() > 0)
        {
            MoveHandler(&currentMove);
        }
        BeginDrawing();
        if (inMotion.load())
        {
            DrawRectangle(0,0,50,50,{128,128,128,255});
        }
        
        ClearBackground(BACKGROUND_COLOR);
        DrawBoard();
        for (int i = 0;i<btns.size();i++)
        {
          btns[i].Draw();
        }
        DrawPieces(BB.load());
        HighlightSq(sq);
        HighlightPlayableMoves(sq);
        EndDrawing();
        
    }

CloseWindow();

}
