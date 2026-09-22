#pragma once
#include <iostream> 
#include <cmath>
#include <cstdint>
#include <vector>
#include <thread>
#include <bits/stdc++.h>
using namespace std;
namespace CheckTools
{
const int B = 0;
const int W = 1;
const int M = 2;
const int K = 3;

struct Bitboard;
struct Move;
Bitboard Makemove(Bitboard BB,Move* mve);

struct Move
{
    vector <char> captureChain;
};
struct Bitboard{
    uint64_t BB[4] = {0,0,0,0};
    bool bturn = 1;
    int half = 0;
    int full = 0;

     Bitboard operator*(Move& mve )
    {
        return Makemove(*this,&mve);
    }
};
struct evalMove
    {
        Move mv;
        double eval;
    };

bool getbit(uint64_t inum,int bitlocus)
            {

            bitlocus = (uint64_t)(bitlocus/8)*8 +(7 -bitlocus%8);
            return (uint64_t )(  (inum>>(bitlocus))& (uint64_t )1 );
            }

int strtoint(string s) 
    {
        int result = 0;
    for (size_t i = 0;i<s.length();i++)
        {
            //cout<<i;
            result += (s[i] - '0')*pow(10,s.length()-i-1);
        }
        return result;
    }

uint64_t setbit(uint64_t inum,int bitlocus,int bit)
        {
        bitlocus = (int)floor(bitlocus/8)*8 +(7 -bitlocus%8);
        if (bit == 1)
            {
            return inum|( (uint64_t)1<<bitlocus  );
            }
        else
            {
            return inum&( ~((uint64_t)1<<bitlocus))                  ;
            } 
        }

bool inrange(int a,int b)
    {
      return (a>=0 && b>=0 && a<8 && b<8);  
    }
bool inrange(int a)
{
    return ( a>=0 && a<=63 );
}

double evaluation(Bitboard BB)    
        {
            if (BB.BB[W] == 0){return -9999;}
            if (BB.BB[B] == 0){return 9999;}
            double result = 0;
            double bMaterial = 0;
            double wMaterial = 0;
            const double k = 5;
            const double m = 1;
            for (int i = 0; i<64;i++)
            {
            bMaterial += (getbit(BB.BB[K],i)*k + getbit(BB.BB[M],i)*m )*(-getbit(BB.BB[B],i)*i*0.4);
            wMaterial +=  (getbit(BB.BB[K],i)*k + getbit(BB.BB[M],i)*m )*(getbit(BB.BB[W],i)*(64-i)*0.4); 
            }
            result += bMaterial+wMaterial;
            result += ( (max(-bMaterial,wMaterial))/min(-bMaterial,wMaterial))*(abs(wMaterial + bMaterial)/(wMaterial + bMaterial));
            return result;
        }

Bitboard FENtoboard(string FEN)
    {
        Bitboard BB;
        if (FEN[0] == 'W'){BB.bturn=1;}
        else if (FEN[0] == 'B'){BB.bturn = 0;}
        else cout<<"Invalid FEN";

        FEN = FEN.substr(2,FEN.length());
        bool icolor = 1;
        bool king = false;
        for (size_t i = 0;i<FEN.length();i++)
            {
                char s = FEN[i];

                switch (s)
                {
                    case 'B':{icolor = 0;break;}
                    case 'W':{icolor = 1;break;}
                    case 'K':{king = true;cout<<"king "<<i<<endl;break;}
                    case ',':{break;}
                    case ':':{break;}
                    case 'H':
                    {
                        BB.half = strtoint(FEN.substr(i+1,2));
                        break;
                    }
                    case 'F':
                    {
                        BB.full = strtoint(FEN.substr(i+1,2));
                        break;
                    }
                   default:
                   {if (s>='a'&&s<='h')
                     {
                       int sq = (s-'a')+(FEN[i+1]-'1')*8;
                     // cout<<icolor<<"--"<<sq<<endl;
                      //sq = floor(7 - floor(sq/8))*8 +sq%8;
                      BB.BB[icolor] = setbit(BB.BB[icolor],sq,1);
                      BB.BB[king + 2] = setbit(BB.BB[king + 2],sq,1);
                      king = false;
                        i++;
                        }
                         break;
                     }
                }
            
            }

            return BB;
    }

Bitboard Makemove(Bitboard BB,char startSqr, char endSqr)
    {
       for (int i = 0;i<4;i++)
            {
            BB.BB[i] = setbit(BB.BB[i],endSqr,getbit(BB.BB[i],startSqr) );//Moving piece to destination
            BB.BB[i] = setbit(BB.BB[i],startSqr,0);//Removing piece from start square
            }
       

        if (abs(startSqr-endSqr)>9)
        {
            int c = (startSqr+endSqr)/2;
            for (int i = 0;i<4;i++)
                {
                    BB.BB[i] = setbit(BB.BB[i],c,0);//Removing captured piece
                }
        }
        
         if ( (floor(endSqr/8) == 7 &&BB.bturn == W) || (floor(endSqr/8) == 0 && BB.bturn == B) ) 
       {
        BB.BB[K] = setbit(BB.BB[K],endSqr,1);//Placing king if the piece lands on the last rank
        BB.BB[M] = setbit(BB.BB[M],endSqr,0);//removing the piece from the man bitboard
        }
        return BB;
    }

Bitboard Makemove(Bitboard BB,vector <int>* captureChain)
    {
    if ((*captureChain).size() == 2)
        {
        BB = Makemove(BB,(*captureChain)[0],(*captureChain)[1]  );
        }
    else if ((*captureChain).size()>2)
        {
        for (size_t i = 0;i<(*captureChain).size()-1;i++)
            {
               BB = Makemove(BB,(*captureChain)[i],(*captureChain)[i+1]);
            }
        }
        BB.bturn = !BB.bturn;
        return BB;
    }

Bitboard Makemove(Bitboard BB,Move* mv)
    {
    if ( (*mv).captureChain.size() == 2)
        {
        BB = Makemove(BB,(*mv).captureChain[0],(*mv).captureChain[1]);
        }
    else if ((*mv).captureChain.size()>2)
        {
        for (size_t i = 0;i<(*mv).captureChain.size()-1;i++)
            {
               BB = Makemove(BB,(*mv).captureChain[i],(*mv).captureChain[i+1]);
            }
        }
        BB.bturn = !BB.bturn;
        return BB;
    }

Move MOVE(char a, char b)
    {
    Move mv;
    mv.captureChain = {a,b};
    return mv;
    }

void printBB(Bitboard BB)
    {
    uint64_t h = BB.BB[W];
    uint64_t k = BB.BB[B];
     for (int l = 0;l<64;l++)
    {
    uint64_t bt = h& ((uint64_t)1)<<63;
   uint64_t bit = k& ((uint64_t)1)<<63;
     bt = bt == 0?0:1;
    bit =bit == 0?0:1;
    cout<<bt+2*bit<<" ";
    if ((l+1)%8==0){cout<<endl;}
     h = h<<1;
     k = k<<1;
    } 
    
    cout<<endl;
    
} 

void DFS (Bitboard BB1, Move mv, int sq,vector <Move>* chain)
            {

         bool bfound = false;
        bool turn = BB1.bturn;
         for (int grad = -1;grad<=1;grad+=2)
            {
                int ihat = sq%8;
                int j = sq/8;

                int i1 = ihat+grad;
                if (!inrange(i1,1)){continue;}

                for  (int jdir = -1;jdir<=1;jdir+=2 )
                    {
                    int j1 = j+jdir;    
                    if (!inrange(i1,j1)){continue;}  
                    if (getbit(BB1.BB[!BB1.bturn],i1+j1*8) )
                        {
                            int endSquare = (i1+grad)+(j1+jdir)*8;//i1+grad,j1+jdir
                        if (  !getbit(BB1.BB[W]|BB1.BB[B],endSquare )&&inrange(i1+grad,j1+jdir))
                            {
                                bfound = true;
                             Move mv2 = mv;
                            if (mv2.captureChain.size() == 0){mv2.captureChain.push_back(sq);}
                            mv2.captureChain.push_back(endSquare);
                            Bitboard p = Makemove(BB1,sq,endSquare);
                            p.bturn = turn;
                            DFS(p,mv2,endSquare,chain);    

                            }
                        }
                    }
            }
            if (bfound == false){(*chain).push_back(mv);}
            };

void Chainer(Bitboard BB , vector <Move>* chain)
    {
        for (int i = 0 ;i<64;i++)
    {
    if (getbit(BB.BB[BB.bturn],i))
        {
            int isKing = getbit(BB.BB[K],i);
            int turn = BB.bturn;
        for (int grad = -1;grad<=1;grad+=2)
            {
                int ihat = i%8;
                int j = i/8;

                int i1 = ihat+grad;
                if (!inrange(i1,1)){continue;}
                for  (int jdir = (2*turn-1)*(1-isKing)-isKing;jdir<=(2*turn-1)*(1-isKing)+isKing;jdir+=2 )//Math bitch
                    {
                    int j1 = j+jdir;    
                    if (!inrange(i1,j1)){continue;}  
                    if (getbit(BB.BB[!BB.bturn],i1+j1*8) )
                        {
                          int  sq = (i1+grad)+(j1+jdir)*8;
                        if (  !getbit(BB.BB[W]|BB.BB[B],sq )&&inrange(i1+grad,j1+jdir))
                            {
                            Move mve;
                            mve.captureChain.push_back(i);
                            mve.captureChain.push_back(sq);
                            Bitboard BB2;
                            BB2 = Makemove(BB,i,sq);
                            BB2.bturn = turn;
                            DFS(BB2, mve, sq,chain);
                            }  
                        }                        
                    }
            }
        }
    }
        

    }

void getMoves(Bitboard BB, vector <Move>* moves)
{
//forced moves
//--------------------------------


Chainer(BB,moves);
if ( (*moves).size() > 0) {return;}


//unforced moves
for (int i = 0 ;i<64;i++)
    {
        
    if (getbit(BB.BB[BB.bturn],i))
        {
            int isKing = getbit(BB.BB[K],i);
            int turn = BB.bturn;
        for (int grad = -1;grad<=1;grad+=2)
            {
                int ihat = i%8;
                int j = (int)(i/8);

                int i1 = ihat+grad;
                if (!inrange(i1,1)){continue;}
                for  (int jdir = (2*turn-1)*(1-isKing)-isKing;jdir<=(2*turn-1)*(1-isKing)+isKing;jdir+=2 )
                    {
                    int j1 = j+jdir; 
                     
                    if (!inrange(i1,j1)){continue;}
                    if (!getbit(BB.BB[W]|BB.BB[B],j1*8+i1)  )
                        {
                            (*moves).push_back(MOVE(i,j1*8+i1));
                        }                        
                    }
            }
        }
    }
}

double miniMax(Bitboard BB,int depth, double alpha, double beta) 
        {
        if ( (depth == 0) || (BB.BB[W] == 0)||(BB.BB[B] == 0) )
        {
        
            return evaluation(BB);
        }
        vector <Move> movelist;
        getMoves(BB,&movelist);
        if ( movelist.size() == 0){return 0;}
        if (BB.bturn)
            {
          double mx = -9999999;

          
          for (size_t i = 0;i<movelist.size();i++)
                {
                    Bitboard BB1 = Makemove(BB, &movelist[i]);
                    double bestfind = miniMax(BB1, depth - 1,alpha,beta);
                    mx = max(mx,bestfind);
                    alpha = max(bestfind, alpha);
                    if (beta<=alpha){break;}
                }
                return mx;
            }
            else 
            {
                double mn = 999999;

          for (size_t i = 0;i<movelist.size();i++)
                {
                    Bitboard BB1 = Makemove(BB, &movelist[i]);
                    double bestfind = miniMax(BB1, depth - 1,alpha,beta);
                    mn = min(mn,bestfind);
                    beta = min(beta,bestfind);
                    if (beta<=alpha){break;}
                }
                return mn;


            }
        
          
        }

void BestMove(Bitboard BB, vector <Move>* movelist,int depth,evalMove* mx)
        {
        
    evalMove mve;
          
       
    if (BB.bturn)
        {
        mve.eval = -9999999;
        //complete this or you die
        for (size_t i = 0;i<(*movelist).size();i++)
            {
                Bitboard BB1 = Makemove(BB, &(*movelist)[i]);
                double bestfind = miniMax(BB1, depth - 1,-99999,99999);
                if ( mve.eval < bestfind ){mve = {(*movelist)[i],bestfind};}
            }
            (*mx) = mve;
               
            }
            else 
            {
                
                mve.eval = 9999999;
                 for (size_t i = 0;i<(*movelist).size();i++)
                {
                    Bitboard BB1 = Makemove(BB, &(*movelist)[i]);
                    double bestfind = miniMax(BB1, depth - 1,-99999,99999);
                    if ( mve.eval > bestfind){mve = {(*movelist)[i],bestfind};}
                }
                (*mx) = mve;
            }
      
        }

evalMove bestMove(Bitboard BB , int depth,int cores)
    {
    thread threads[cores];       
    vector <Move> movelist;
    evalMove results[cores];
    for (int i = 0;i<cores;i++)
        {
            results[i].eval = (2*BB.bturn-1)*-99999;
            results[i].mv= MOVE(-1,-1);
        }
    getMoves(BB,&movelist);
    int moveCount = movelist.size();
    vector <Move> threadMoveList[cores];
    for (int i = 0; i<cores;i++)      
        {
        copy(movelist.begin() + (moveCount*i)/cores,
     movelist.begin() + (moveCount*(i+1))/cores,
     back_inserter(threadMoveList[i]));
        }
    for (int i = 0;i<cores;i++)
        {
            if (threadMoveList[i].size() == 0){threads[i] = thread();}
            else
           {
            threads[i] = thread(BestMove,BB,&threadMoveList[i],depth,&results[i]);
            }
        }
       for (int i = 0;i<cores;i++)
        {
            if (threads[i].joinable() == false ){continue;}
            threads[i].join();
        }
        evalMove mnx;
        mnx.eval = (2*BB.bturn-1)*-99999;
        for (int i = 0;i<cores;i++)
            {
                if (BB.bturn)
                    {
                        if (mnx.eval<results[i].eval)
                            {
                                mnx = results[i];
                            }
                        
                    }
                else
                    {
                         if (mnx.eval>results[i].eval)
                            {
                                mnx = results[i];
                            }
                    }
            }
      
        return mnx;
    }

void printMove(Move* mv)
    {
        cout<<endl;
        for (size_t j = 0;j<(*mv).captureChain.size();j++)
            {
                cout<<(int)(*mv).captureChain[j]<<" ";
            }
            cout<<endl;
    }
}