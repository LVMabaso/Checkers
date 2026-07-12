#include <iostream> 
#include <cmath>
#include <cstdint>
#include <vector>

using namespace std;
const int B = 0;
const int W = 1;
const int M = 2;
const int K = 3;



int strtoint(string s) 
    {
        int result = 0;
    for (int i = 0;i<s.length();i++)
        {
            //cout<<i;
            result += (s[i] - '0')*pow(10,s.length()-i-1);
        }
        return result;
    }
struct Bitboard{
    uint64_t BB[4] = {0,0,0,0};
    bool bturn = 1;
    int half = 0;
    int full = 0;

}                  ; 
struct Move
{
    int a = 0;
    int b = 0;
    bool c = 0;
    vector <int> captureChain;
};
bool inrange(int a,int b)
    {
      return (a>=0 && b>=0 && a<8 && b<8);  
    }
bool inrange(int a){
    return ( a>=0 && a<=63 );
}

bool getbit(uint64_t inum,int bitlocus)
            {

            bitlocus = (int)floor(bitlocus/8)*8 +(7 -bitlocus%8);
            return (int)(  (inum>>(bitlocus))& (long)1 );
            }
    


uint64_t setbit(uint64_t inum,int bitlocus,int bit)
        {
        bitlocus = (int)floor(bitlocus/8)*8 +(7 -bitlocus%8);
        if (bit == 1)
            {
            return inum|( (long)1<<bitlocus  );
            }
        else
            {
            return inum&( ~((long)1<<bitlocus))                  ;
            } 
        }


double evaluation(Bitboard BB)    
        {
            double result = 0;
            const double k = 3;
            const double m = 1;
            for (int i = 0; i<64;i++)
            {
            result += (getbit(BB.BB[K],i)*k + getbit(BB.BB[M],i)*m )*(getbit(BB.BB[W],i) - getbit(BB.BB[B],i) );
            }
            if (BB.BB[W] == (uint64_t)0){return -9999;}
            if (BB.BB[B] == (uint64_t)0){return 9999;}
            return result;
        }


vector <Move> movelist(Bitboard BB)
    {
        vector <Move> mv;
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
        for (int i = 0;i<FEN.length();i++)
            {
                char s = FEN[i];

                switch (s)
                {
                    case 'B':{icolor = 0;break;}
                    case 'W':{icolor = 1;break;}
                    case 'k':{king = true;cout<<"king"<<endl;break;}
                    case ',':{break;};
                    case ':':{break;};
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
                      cout<<icolor<<"--"<<sq<<endl;
                      sq = floor(7 - floor(sq/8))*8 +sq%8;
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



int main(){
Bitboard BB = FENtoboard("B:Wd4,a3,e3,kg3,b2,d2,f2,h2,a1,c1,e1,g1:Bb8,d8,f8,h8,a7,c7,Ke7,g7,b6,d6,f6,h6");
uint64_t itest = 101;
itest = setbit(itest,2,0);;
cout<<"Integer = "<<itest<<endl;
for (int i = 0;i<64;i++)
    {
        cout<<getbit(BB.BB[0]|BB.BB[1],i)<<" ";
        if ((i+1)%8==0){cout<<endl;}
    }

    cout<<endl<<"vectors ";
      
vector <Move> mvlist = movelist(BB);
mvlist.push
for (int i = 0;i<mvlist.size();i++)
    {
        cout<<mvlist[i].a<<" - "<<mvlist[i].b<<" - "<<mvlist[i].c<<endl;
    }
}
