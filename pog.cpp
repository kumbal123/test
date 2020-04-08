#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */

class CTransaction{
  public:
    int m_Amount;
    char * m_OtherAccID;
    char * m_Sign;
    char m_mark;

    CTransaction(): m_Amount(0),
                    m_OtherAccID(nullptr),
                    m_Sign(nullptr){}

    CTransaction( int amount, const char * otherID, const char * sign, char mark ){
        m_Amount = amount;
        m_OtherAccID = new char [strlen(otherID) + 1];
        strcpy(m_OtherAccID, otherID);
        m_Sign = new char [ strlen(sign) + 1];
        strcpy(m_Sign, sign);
        m_mark = mark;
    }
};
//---------------------------------------------------------------------------------------------------------------------------
class CAcc{
  public:
    char * m_AccID;
    int m_InitialBalance;
    int m_Balance;
    CTransaction * m_Transaction;
    int m_SizeTrans;
    int m_MaxTrans;

    CAcc(): m_AccID(nullptr),
            m_InitialBalance(0),
            m_Balance(0),
            m_Transaction(nullptr),
            m_SizeTrans(0),
            m_MaxTrans(0){}

    CAcc( const char * accID, int initialBalance ){
        m_AccID = new char[strlen(accID) + 1];
        strcpy( m_AccID, accID );
        m_InitialBalance = initialBalance;
        m_Balance = initialBalance;
        m_Transaction = nullptr;
        m_SizeTrans = 0;
        m_MaxTrans = 0;
    }

    void reallocTrans( int required ){
        if( m_MaxTrans == required ){
            m_MaxTrans += m_MaxTrans < 100 ? 10 : m_MaxTrans/2;
            CTransaction * tmp = new CTransaction[m_MaxTrans];
            for( int i = 0; i < m_SizeTrans; ++i ){
                CTransaction tmpTrans = CTransaction( m_Transaction[i].m_Amount, m_Transaction[i].m_OtherAccID,
                                                      m_Transaction[i].m_Sign, m_Transaction[i].m_mark );
                tmp[i] = tmpTrans;
            }
            for( int i = 0; i < m_SizeTrans; ++i ){
                delete [] m_Transaction[i].m_OtherAccID;
                delete [] m_Transaction[i].m_Sign;
            }
            delete [] m_Transaction;
            m_Transaction = tmp;
        }
    }

    void AddTransaction ( int amount, const char * accID, const char * sign, char mark );
    int Balance() const;
};

void CAcc::AddTransaction ( int amount, const char * accID, const char * sign, char mark ){
    reallocTrans( m_SizeTrans );
    m_Transaction[m_SizeTrans] = CTransaction(amount, accID, sign, mark );
    m_SizeTrans++;
}

int CAcc::Balance() const{
    return m_Balance;
}
//---------------------------------------------------------------------------------------------------------------------------
class CBridge{
  public:
    CAcc * m_Accounts;
    int m_SizeAcc;
    int m_MaxAcc;
    unsigned int m_SumPointer;

    CBridge(): m_Accounts(nullptr),
               m_SizeAcc(0),
               m_MaxAcc(0),
               m_SumPointer(1){}

    CBridge * copy( void ) const;

};

CBridge * CBridge::copy() const{
    CBridge * tmpbridge = new CBridge();
    tmpbridge ->  m_SizeAcc = m_SizeAcc;
    tmpbridge ->  m_MaxAcc = m_MaxAcc;
    tmpbridge ->  m_SumPointer = 1;
    CAcc * tmpAcc = new CAcc[ m_MaxAcc ];
    for( int i = 0; i < m_SizeAcc; ++i ){
        CAcc tmp = CAcc( m_Accounts[i].m_AccID, m_Accounts[i].m_InitialBalance );
        tmp.m_Balance = m_Accounts[i].m_Balance;
        tmp.m_SizeTrans = m_Accounts[i].m_SizeTrans;
        tmp.m_MaxTrans = m_Accounts[i].m_MaxTrans;
        CTransaction * tmpTrans = new CTransaction[ m_Accounts[i].m_MaxTrans ];
        for( int j = 0; j < m_Accounts[i].m_SizeTrans; ++j ){
            CTransaction tmp2 = CTransaction( m_Accounts[i].m_Transaction[j].m_Amount,
                                             m_Accounts[i].m_Transaction[j].m_OtherAccID,
                                             m_Accounts[i].m_Transaction[j].m_Sign,
                                             m_Accounts[i].m_Transaction[j].m_mark );
            tmpTrans[j] = tmp2;
        }
        tmp.m_Transaction = tmpTrans;
        tmpAcc[i] = tmp;
    }
    tmpbridge ->  m_Accounts = tmpAcc;
    return tmpbridge;
}
//---------------------------------------------------------------------------------------------------------------------------
class CBank{
  private:
    CBridge * bridge;
  public:
    // default constructor
    CBank(){
        bridge = new CBridge();
    }
    // copy constructor
    CBank( CBank & x ){
        x.bridge ->  m_SumPointer++;
        bridge = x.bridge;
    }
    // destructor
    ~CBank(){
        if( bridge -> m_SumPointer == 1 ){
            for( int i = 0; i < (bridge -> m_SizeAcc); ++i ){
                for( int j = 0; j < (bridge -> m_Accounts[i].m_SizeTrans); ++j ){
                    delete [] (bridge ->  m_Accounts[i].m_Transaction[j].m_OtherAccID);
                    delete [] (bridge -> m_Accounts[i].m_Transaction[j].m_Sign);
                }
                delete [] (bridge -> m_Accounts[i].m_Transaction);
                delete [] (bridge -> m_Accounts[i].m_AccID);
            }
            delete [] (bridge -> m_Accounts);
            delete [] bridge;
        }else if( bridge -> m_SumPointer > 1 ){
            bridge -> m_SumPointer--;
            bridge = nullptr;
        }
    }
    // operator =
    void operator = ( CBank & x );

    void realloc( CBridge * bridge ){
        if( bridge -> m_MaxAcc == bridge -> m_SizeAcc ){
            bridge -> m_MaxAcc += bridge -> m_MaxAcc < 100 ? 10 : bridge -> m_MaxAcc/2;
            CAcc * tmp = new CAcc[ bridge -> m_MaxAcc ];
            for( int i = 0; i < bridge -> m_SizeAcc; ++i ){
                CAcc tmpAcc = CAcc( bridge -> m_Accounts[i].m_AccID, bridge -> m_Accounts[i].m_InitialBalance );
                tmpAcc.m_Balance = bridge -> m_Accounts[i].m_Balance;
                tmpAcc.m_SizeTrans = bridge -> m_Accounts[i].m_SizeTrans;
                tmpAcc.m_MaxTrans = bridge -> m_Accounts[i].m_MaxTrans;
                CTransaction * tmpTrans = new CTransaction[ tmpAcc.m_MaxTrans ];
                for( int j = 0; j < tmpAcc.m_SizeTrans; ++j ){
                    CTransaction tmp2 = CTransaction( bridge -> m_Accounts[i].m_Transaction[j].m_Amount,
                                                      bridge -> m_Accounts[i].m_Transaction[j].m_OtherAccID,
                                                      bridge -> m_Accounts[i].m_Transaction[j].m_Sign,
                                                      bridge -> m_Accounts[i].m_Transaction[j].m_mark );
                    tmpTrans[j] = tmp2;
                }
                tmpAcc.m_Transaction = tmpTrans;
                tmp[i] = tmpAcc;
            }
            for( int i = 0; i < (bridge -> m_SizeAcc); ++i ){
                for( int j = 0; j < (bridge -> m_Accounts[i].m_SizeTrans); ++j ){
                    delete [] (bridge ->  m_Accounts[i].m_Transaction[j].m_OtherAccID);
                    delete [] (bridge -> m_Accounts[i].m_Transaction[j].m_Sign);
                }
                delete [] (bridge -> m_Accounts[i].m_Transaction);
                delete [] (bridge -> m_Accounts[i].m_AccID);
            }
            delete [] (bridge -> m_Accounts);
            bridge -> m_Accounts = tmp;
        }
    }
    bool   NewAccount    ( const char * accID,
                           int          initialBalance );
    bool   Transaction   ( const char * debAccID,
                           const char * credAccID,
                           unsigned int amount,
                           const char * signature );
    bool   TrimAccount   ( const char * accID );
    // Account ( accID )
    CAcc & Account ( const char * accID ) const;

    friend ostream & operator << ( ostream & out, CAcc & x );
};

void CBank::operator = ( CBank & x ){

    if( this != &x ){
        if( bridge -> m_SumPointer > 1 ){
            bridge -> m_SumPointer--;
        }else if( bridge -> m_SumPointer == 1 ){
            for( int i = 0; i < bridge ->   m_SizeAcc; ++i ){
                for( int j = 0; j < bridge ->  m_Accounts[i].m_SizeTrans; ++j ){
                    delete [] bridge ->  m_Accounts[i].m_Transaction[j].m_OtherAccID;
                    delete [] bridge -> m_Accounts[i].m_Transaction[j].m_Sign;
                }
                delete [] bridge -> m_Accounts[i].m_Transaction;
                delete [] bridge -> m_Accounts[i].m_AccID;
            }
            delete [] bridge -> m_Accounts;
            delete [] bridge;
        }
        x.bridge -> m_SumPointer++;
        bridge = x.bridge;
    }
}

bool   CBank::NewAccount ( const char * accID, int initialBalance ){
    for( int i = 0; i < bridge -> m_SizeAcc; ++i ){
        if( !strcmp( accID, bridge -> m_Accounts[i].m_AccID ) )
            return false;
    }

    if( bridge -> m_SumPointer > 1 ){
        bridge -> m_SumPointer--;
        bridge = bridge -> copy();
    }

    realloc( bridge );
    bridge -> m_Accounts[bridge -> m_SizeAcc] = CAcc( accID, initialBalance );
    bridge -> m_SizeAcc++;
    return true;
}

bool   CBank::Transaction ( const char * debAccID, const char * credAccID, unsigned int amount, const char * signature ){
    int debAccPos = -1, creadAccPos = -1;
    if( !strcmp( debAccID, credAccID ) )
        return false;
    for( int i = 0; i < bridge -> m_SizeAcc; ++i ){
        if( debAccPos != -1 && creadAccPos != -1 )
            break;

        if( !strcmp( bridge -> m_Accounts[i].m_AccID, debAccID ) ){
            debAccPos = i;
        }else if( !strcmp( bridge -> m_Accounts[i].m_AccID, credAccID ) ){
            creadAccPos = i;
        }
    }

    if( debAccPos == -1 || creadAccPos == -1 )
        return false;
    
    if( bridge -> m_SumPointer > 1 ){
        bridge -> m_SumPointer--;
        bridge = bridge -> copy();
    }

    bridge -> m_Accounts[ debAccPos ].m_Balance -= amount;
    bridge -> m_Accounts[ creadAccPos ].m_Balance += amount;
    bridge -> m_Accounts[ debAccPos ].AddTransaction( amount, credAccID, signature, '-' );
    bridge -> m_Accounts[ creadAccPos ].AddTransaction( amount, debAccID, signature, '+' );
    return true;
}

bool   CBank::TrimAccount   ( const char * accID ){
    int accIDPos = -1;
    for( int i = 0; i < bridge -> m_SizeAcc; ++i ){
        if( !strcmp( accID, bridge -> m_Accounts[i].m_AccID ) ){
            accIDPos = i;
            break;
        }
    }

    if( accIDPos == -1 )
        return false;
    
    if( bridge -> m_SumPointer > 1 ){
        bridge -> m_SumPointer--;
        bridge = bridge -> copy();
    }

    for( int j = 0; j < bridge -> m_Accounts[ accIDPos ].m_SizeTrans; ++j ){
        delete [] bridge -> m_Accounts[ accIDPos ].m_Transaction[j].m_OtherAccID;
        delete [] bridge -> m_Accounts[ accIDPos ].m_Transaction[j].m_Sign;
    }

    delete [] bridge -> m_Accounts[ accIDPos ].m_Transaction;

    bridge -> m_Accounts[ accIDPos ].m_Transaction = nullptr;
    bridge -> m_Accounts[ accIDPos ].m_SizeTrans = 0;
    bridge -> m_Accounts[ accIDPos ].m_MaxTrans = 0;
    bridge -> m_Accounts[ accIDPos ].m_InitialBalance = bridge -> m_Accounts[ accIDPos ].m_Balance;
    return true;
}

CAcc & CBank::Account ( const char * accID ) const{
    int accIDPos = -1;
    for( int i = 0; i < bridge -> m_SizeAcc; ++i ){
        if( !strcmp( accID, bridge -> m_Accounts[i].m_AccID ) ){
            accIDPos = i;
            break;
        }
    }
    
    if( accIDPos == -1 )
        throw runtime_error("acID nenalezeno");

    return bridge -> m_Accounts[ accIDPos ];    
}

ostream & operator << ( ostream & out, CAcc & x ){
    out << x.m_AccID << ":\n   " << x.m_InitialBalance << "\n";
    for( int i = 0; i < x.m_SizeTrans; ++i ){
        out << " " << x.m_Transaction[i].m_mark << " " << x.m_Transaction[i].m_Amount << ", ";
        if( x.m_Transaction[i].m_mark == '+' )
            out << "from: ";
        else
            out << "to: ";
        out << x.m_Transaction[i].m_OtherAccID << ", sign: " << x.m_Transaction[i].m_Sign << "\n";
    }
    out << " = " << x.m_Balance << "\n";
    return out;
}


#ifndef __PROGTEST__
int main ( void )
{
CBank KB;    ostringstream os;
//----------------------- hodne dat test ------------------------------------------------------------------------
    {
        assert ( KB . NewAccount ( "123456", 1000 ) );
        assert ( KB . NewAccount ( "123457", 1000 ) );
        assert ( KB . NewAccount ( "123458", 1000 ) );
        assert ( KB . NewAccount ( "123459", 1000 ) );
        assert ( KB . NewAccount ( "123460", 1000 ) );
        assert ( KB . NewAccount ( "123461", 1000 ) );
        assert ( KB . NewAccount ( "123462", 1000 ) );
        assert ( KB . NewAccount ( "123463", 1000 ) );
        assert ( KB . NewAccount ( "123464", 1000 ) );
        assert ( KB . NewAccount ( "123465", 1000 ) );
        assert ( KB . NewAccount ( "123466", 1000 ) );
        assert ( KB . NewAccount ( "123467", 1000 ) );
        assert ( KB . NewAccount ( "123468", 1000 ) );
        assert ( KB . NewAccount ( "123469", 1000 ) );
        assert ( KB . NewAccount ( "123470", 1000 ) );
        assert ( KB . NewAccount ( "123471", 1000 ) );
        assert ( KB . NewAccount ( "123472", 1000 ) );
        assert ( KB . NewAccount ( "123473", 1000 ) );
        assert ( KB . NewAccount ( "123474", 1000 ) );
        assert ( KB . NewAccount ( "123475", 1000 ) );
        assert ( KB . NewAccount ( "123476", 1000 ) );
        assert ( KB . NewAccount ( "123477", 1000 ) );
        assert ( KB . NewAccount ( "123478", 1000 ) );
        assert ( KB . NewAccount ( "123479", 1000 ) );
        assert ( KB . NewAccount ( "123480", 1000 ) );
        assert ( KB . NewAccount ( "123481", 1000 ) );
        assert ( KB . NewAccount ( "123482", 1000 ) );
        assert ( KB . NewAccount ( "123483", 1000 ) );
        assert ( KB . NewAccount ( "123484", 1000 ) );
        assert ( KB . NewAccount ( "123485", 1000 ) );
        assert ( KB . NewAccount ( "123486", 1000 ) );
        assert ( KB . NewAccount ( "123487", 1000 ) );
        assert ( KB . NewAccount ( "123488", 1000 ) );
        assert ( KB . NewAccount ( "123489", 1000 ) );
        assert ( KB . NewAccount ( "123490", 1000 ) );
        assert ( KB . NewAccount ( "123491", 1000 ) );
        assert ( KB . NewAccount ( "123492", 1000 ) );
        assert ( KB . NewAccount ( "123493", 1000 ) );
        assert ( KB . NewAccount ( "123494", 1000 ) );
        assert ( KB . NewAccount ( "123495", 1000 ) );
        assert ( KB . NewAccount ( "123496", 1000 ) );
        assert ( KB . NewAccount ( "123497", 1000 ) );
        assert ( KB . NewAccount ( "123498", 1000 ) );
        assert ( KB . NewAccount ( "123499", 1000 ) );
        assert ( KB . NewAccount ( "123500", 1000 ) );
        assert ( KB . NewAccount ( "123501", 1000 ) );
        assert ( KB . NewAccount ( "123502", 1000 ) );
        assert ( KB . NewAccount ( "123503", 1000 ) );
        assert ( KB . NewAccount ( "123504", 1000 ) );
        assert ( KB . NewAccount ( "123505", 1000 ) );
        assert ( KB . NewAccount ( "123506", 1000 ) );
        assert ( KB . NewAccount ( "123507", 1000 ) );
        assert ( KB . NewAccount ( "123508", 1000 ) );
        assert ( KB . NewAccount ( "123509", 1000 ) );
        assert ( KB . NewAccount ( "123510", 1000 ) );
        assert ( KB . NewAccount ( "123511", 1000 ) );
        assert ( KB . NewAccount ( "123512", 1000 ) );
        assert ( KB . NewAccount ( "123513", 1000 ) );
        assert ( KB . NewAccount ( "123514", 1000 ) );
        assert ( KB . NewAccount ( "123515", 1000 ) );
        assert ( KB . NewAccount ( "123516", 1000 ) );
        assert ( KB . NewAccount ( "123517", 1000 ) );
        assert ( KB . NewAccount ( "123518", 1000 ) );
        assert ( KB . NewAccount ( "123519", 1000 ) );
        assert ( KB . NewAccount ( "123520", 1000 ) );
        assert ( KB . NewAccount ( "123521", 1000 ) );
        assert ( KB . NewAccount ( "123522", 1000 ) );
        assert ( KB . NewAccount ( "123523", 1000 ) );
        assert ( KB . NewAccount ( "123524", 1000 ) );
        assert ( KB . NewAccount ( "123525", 1000 ) );
        assert ( KB . NewAccount ( "123526", 1000 ) );
        assert ( KB . NewAccount ( "123527", 1000 ) );
        assert ( KB . NewAccount ( "123528", 1000 ) );
        assert ( KB . NewAccount ( "123529", 1000 ) );
        assert ( KB . NewAccount ( "123530", 1000 ) );
        assert ( KB . NewAccount ( "123531", 1000 ) );
        assert ( KB . NewAccount ( "123532", 1000 ) );
        assert ( KB . NewAccount ( "123533", 1000 ) );
        assert ( KB . NewAccount ( "123534", 1000 ) );
        assert ( KB . NewAccount ( "123535", 1000 ) );
        assert ( KB . NewAccount ( "123536", 1000 ) );
        assert ( KB . NewAccount ( "123537", 1000 ) );
        assert ( KB . NewAccount ( "123538", 1000 ) );
        assert ( KB . NewAccount ( "123539", 1000 ) );
        assert ( KB . NewAccount ( "123540", 1000 ) );
        assert ( KB . NewAccount ( "123541", 1000 ) );
        assert ( KB . NewAccount ( "123542", 1000 ) );
        assert ( KB . NewAccount ( "123543", 1000 ) );
        assert ( KB . NewAccount ( "123544", 1000 ) );
        assert ( KB . NewAccount ( "123545", 1000 ) );
        assert ( KB . NewAccount ( "123546", 1000 ) );
        assert ( KB . NewAccount ( "123547", 1000 ) );
        assert ( KB . NewAccount ( "123548", 1000 ) );
        assert ( KB . NewAccount ( "123549", 1000 ) );
        assert ( KB . NewAccount ( "123550", 1000 ) );
        assert ( KB . NewAccount ( "123551", 1000 ) );
        assert ( KB . NewAccount ( "123552", 1000 ) );
        assert ( KB . NewAccount ( "123553", 1000 ) );
        assert ( KB . NewAccount ( "123554", 1000 ) );
        assert ( KB . NewAccount ( "123555", 1000 ) );
        assert ( KB . NewAccount ( "123556", 1000 ) );
        assert ( KB . NewAccount ( "123557", 1000 ) );
        assert ( KB . NewAccount ( "123558", 1000 ) );
        assert ( KB . NewAccount ( "123559", 1000 ) );
        assert ( KB . NewAccount ( "123560", 1000 ) );
        assert ( KB . NewAccount ( "123561", 1000 ) );
        assert ( KB . NewAccount ( "123562", 1000 ) );
        assert ( KB . NewAccount ( "123563", 1000 ) );
        assert ( KB . NewAccount ( "123564", 1000 ) );
        assert ( KB . NewAccount ( "123565", 1000 ) );
        assert ( KB . NewAccount ( "123566", 1000 ) );
        assert ( KB . NewAccount ( "123567", 1000 ) );
        assert ( KB . NewAccount ( "123568", 1000 ) );
        assert ( KB . NewAccount ( "123569", 1000 ) );
        assert ( KB . NewAccount ( "123570", 1000 ) );
        assert ( KB . NewAccount ( "123571", 1000 ) );
        assert ( KB . NewAccount ( "123572", 1000 ) );
        assert ( KB . NewAccount ( "123573", 1000 ) );
        assert ( KB . NewAccount ( "123574", 1000 ) );

        assert ( KB . Transaction ( "123456", "123457", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123458", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123459", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123460", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123461", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123462", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123463", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123464", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123465", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123466", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123467", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123468", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123469", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123470", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123471", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123472", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123473", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123474", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123475", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123476", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123477", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123478", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123479", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123480", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123481", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123482", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123483", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123484", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123485", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123486", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123487", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123488", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123489", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123490", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123491", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123492", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123493", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123494", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123495", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123496", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123497", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123498", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123499", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123500", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123501", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123502", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123503", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123504", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123505", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123506", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123507", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123508", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123509", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123510", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123511", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123512", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123513", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123514", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123515", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123516", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123517", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123518", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123519", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123520", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123521", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123522", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123523", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123524", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123525", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123526", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123527", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123528", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123529", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123530", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123531", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123532", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123533", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123534", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123535", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123536", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123537", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123538", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123539", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123540", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123541", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123542", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123543", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123544", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123545", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123546", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123547", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123548", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123549", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123550", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123551", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123552", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123553", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123554", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123555", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123556", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123557", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123558", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123559", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123560", 300, "XAbG5uKz6E=" ) );
        assert ( KB . Transaction ( "123456", "123561", 300, "XAbG5uKz6E=" ) );

        KB = KB;

        CBank x1; 
        assert ( x1 . NewAccount ( "123456", 1000 ) );
        assert ( x1 . NewAccount ( "123457", 1000 ) );
        assert ( x1 . NewAccount ( "123458", 1000 ) );
        assert ( x1 . NewAccount ( "123459", 1000 ) );
        assert ( x1 . NewAccount ( "123460", 1000 ) );
        assert ( x1 . NewAccount ( "123461", 1000 ) );
        assert ( x1 . NewAccount ( "123462", 1000 ) );
        assert ( x1 . NewAccount ( "123463", 1000 ) );
        assert ( x1 . NewAccount ( "123464", 1000 ) );
        assert ( x1 . NewAccount ( "123465", 1000 ) );
        assert ( x1 . NewAccount ( "123466", 1000 ) );
        assert ( x1 . NewAccount ( "123467", 1000 ) );
        assert ( x1 . NewAccount ( "123468", 1000 ) );
        assert ( x1 . NewAccount ( "123469", 1000 ) );
        assert ( x1 . NewAccount ( "123470", 1000 ) );
        assert ( x1 . NewAccount ( "123471", 1000 ) );
        assert ( x1 . NewAccount ( "123472", 1000 ) );
        assert ( x1 . NewAccount ( "123473", 1000 ) );
        assert ( x1 . NewAccount ( "123474", 1000 ) );
        assert ( x1 . NewAccount ( "123475", 1000 ) );
        assert ( x1 . NewAccount ( "123476", 1000 ) );
        assert ( x1 . NewAccount ( "123477", 1000 ) );
        assert ( x1 . NewAccount ( "123478", 1000 ) );
        assert ( x1 . NewAccount ( "123479", 1000 ) );
        assert ( x1 . NewAccount ( "123480", 1000 ) );
        assert ( x1 . NewAccount ( "123481", 1000 ) );
        assert ( x1 . NewAccount ( "123482", 1000 ) );
        assert ( x1 . NewAccount ( "123483", 1000 ) );
        assert ( x1 . NewAccount ( "123484", 1000 ) );
        assert ( x1 . NewAccount ( "123485", 1000 ) );
        assert ( x1 . NewAccount ( "123486", 1000 ) );
        assert ( x1 . NewAccount ( "123487", 1000 ) );
        assert ( x1 . NewAccount ( "123488", 1000 ) );
        assert ( x1 . NewAccount ( "123489", 1000 ) );
        assert ( x1 . NewAccount ( "123490", 1000 ) );
        assert ( x1 . NewAccount ( "123491", 1000 ) );
        assert ( x1 . NewAccount ( "123492", 1000 ) );
        assert ( x1 . NewAccount ( "123493", 1000 ) );
        assert ( x1 . NewAccount ( "123494", 1000 ) );
        assert ( x1 . NewAccount ( "123495", 1000 ) );
        assert ( x1 . NewAccount ( "123496", 1000 ) );
        assert ( x1 . NewAccount ( "123497", 1000 ) );
        assert ( x1 . NewAccount ( "123498", 1000 ) );
        assert ( x1 . NewAccount ( "123499", 1000 ) );
        assert ( x1 . NewAccount ( "123500", 1000 ) );
        assert ( x1 . NewAccount ( "123501", 1000 ) );
        assert ( x1 . NewAccount ( "123502", 1000 ) );
        assert ( x1 . NewAccount ( "123503", 1000 ) );
        assert ( x1 . NewAccount ( "123504", 1000 ) );
        assert ( x1 . NewAccount ( "123505", 1000 ) );
        assert ( x1 . NewAccount ( "123506", 1000 ) );
        assert ( x1 . NewAccount ( "123507", 1000 ) );
        assert ( x1 . NewAccount ( "123508", 1000 ) );
        assert ( x1 . NewAccount ( "123509", 1000 ) );
        assert ( x1 . NewAccount ( "123510", 1000 ) );
        assert ( x1 . NewAccount ( "123511", 1000 ) );
        assert ( x1 . NewAccount ( "123512", 1000 ) );
        assert ( x1 . NewAccount ( "123513", 1000 ) );
        assert ( x1 . NewAccount ( "123514", 1000 ) );
        assert ( x1 . NewAccount ( "123515", 1000 ) );
        assert ( x1 . NewAccount ( "123516", 1000 ) );
        assert ( x1 . NewAccount ( "123517", 1000 ) );
        assert ( x1 . NewAccount ( "123518", 1000 ) );
        assert ( x1 . NewAccount ( "123519", 1000 ) );
        assert ( x1 . NewAccount ( "123520", 1000 ) );
        assert ( x1 . NewAccount ( "123521", 1000 ) );
        assert ( x1 . NewAccount ( "123522", 1000 ) );
        assert ( x1 . NewAccount ( "123523", 1000 ) );
        assert ( x1 . NewAccount ( "123524", 1000 ) );
        assert ( x1 . NewAccount ( "123525", 1000 ) );
        assert ( x1 . NewAccount ( "123526", 1000 ) );
        assert ( x1 . NewAccount ( "123527", 1000 ) );
        assert ( x1 . NewAccount ( "123528", 1000 ) );
        assert ( x1 . NewAccount ( "123529", 1000 ) );
        assert ( x1 . NewAccount ( "123530", 1000 ) );
        assert ( x1 . NewAccount ( "123531", 1000 ) );
        assert ( x1 . NewAccount ( "123532", 1000 ) );
        assert ( x1 . NewAccount ( "123533", 1000 ) );
        assert ( x1 . NewAccount ( "123534", 1000 ) );
        assert ( x1 . NewAccount ( "123535", 1000 ) );
        assert ( x1 . NewAccount ( "123536", 1000 ) );
        assert ( x1 . NewAccount ( "123537", 1000 ) );
        assert ( x1 . NewAccount ( "123538", 1000 ) );
        assert ( x1 . NewAccount ( "123539", 1000 ) );
        assert ( x1 . NewAccount ( "123540", 1000 ) );
        assert ( x1 . NewAccount ( "123541", 1000 ) );
        assert ( x1 . NewAccount ( "123542", 1000 ) );
        assert ( x1 . NewAccount ( "123543", 1000 ) );
        assert ( x1 . NewAccount ( "123544", 1000 ) );
        assert ( x1 . NewAccount ( "123545", 1000 ) );
        assert ( x1 . NewAccount ( "123546", 1000 ) );
        assert ( x1 . NewAccount ( "123547", 1000 ) );
        assert ( x1 . NewAccount ( "123548", 1000 ) );
        assert ( x1 . NewAccount ( "123549", 1000 ) );
        assert ( x1 . NewAccount ( "123550", 1000 ) );
        assert ( x1 . NewAccount ( "123551", 1000 ) );
        assert ( x1 . NewAccount ( "123552", 1000 ) );
        assert ( x1 . NewAccount ( "123553", 1000 ) );
        assert ( x1 . NewAccount ( "123554", 1000 ) );
        assert ( x1 . NewAccount ( "123555", 1000 ) );
        assert ( x1 . NewAccount ( "123556", 1000 ) );
        assert ( x1 . NewAccount ( "123557", 1000 ) );
        assert ( x1 . NewAccount ( "123558", 1000 ) );
        assert ( x1 . NewAccount ( "123559", 1000 ) );
        assert ( x1 . NewAccount ( "123560", 1000 ) );
        assert ( x1 . NewAccount ( "123561", 1000 ) );
        assert ( x1 . NewAccount ( "123562", 1000 ) );
        assert ( x1 . NewAccount ( "123563", 1000 ) );
        assert ( x1 . NewAccount ( "123564", 1000 ) );
        assert ( x1 . NewAccount ( "123565", 1000 ) );
        assert ( x1 . NewAccount ( "123566", 1000 ) );
        assert ( x1 . NewAccount ( "123567", 1000 ) );
        assert ( x1 . NewAccount ( "123568", 1000 ) );
        assert ( x1 . NewAccount ( "123569", 1000 ) );
        assert ( x1 . NewAccount ( "123570", 1000 ) );
        assert ( x1 . NewAccount ( "123571", 1000 ) );
        assert ( x1 . NewAccount ( "123572", 1000 ) );
        assert ( x1 . NewAccount ( "123573", 1000 ) );
        assert ( x1 . NewAccount ( "123574", 1000 ) );

        assert ( x1 . Transaction ( "123456", "123457", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123458", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123459", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123460", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123461", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123462", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123463", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123464", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123465", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123466", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123467", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123468", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123469", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123470", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123471", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123472", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123473", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123474", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123475", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123476", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123477", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123478", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123479", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123480", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123481", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123482", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123483", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123484", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123485", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123486", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123487", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123488", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123489", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123490", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123491", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123492", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123493", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123494", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123495", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123496", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123497", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123498", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123499", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123500", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123501", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123502", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123503", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123504", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123505", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123506", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123507", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123508", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123509", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123510", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123511", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123512", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123513", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123514", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123515", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123516", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123517", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123518", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123519", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123520", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123521", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123522", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123523", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123524", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123525", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123526", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123527", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123528", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123529", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123530", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123531", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123532", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123533", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123534", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123535", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123536", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123537", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123538", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123539", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123540", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123541", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123542", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123543", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123544", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123545", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123546", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123547", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123548", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123549", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123550", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123551", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123552", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123553", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123554", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123555", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123556", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123557", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123558", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123559", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123560", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123561", 300, "XAbG5uKz6E=" ) );

        KB = x1;
        x1 = KB;

        x1 = x1;
        KB = KB;
        CBank x2;
        x1 = x2;
        assert ( x1 . NewAccount ( "123456", 1000 ) );
        assert ( x1 . NewAccount ( "123457", 1000 ) );
        assert ( x1 . NewAccount ( "123458", 1000 ) );
        assert ( x1 . NewAccount ( "123459", 1000 ) );
        assert ( x1 . NewAccount ( "123460", 1000 ) );
        assert ( x1 . NewAccount ( "123461", 1000 ) );
        assert ( x1 . NewAccount ( "123462", 1000 ) );
        assert ( x1 . NewAccount ( "123463", 1000 ) );
        assert ( x1 . NewAccount ( "123464", 1000 ) );
        assert ( x1 . NewAccount ( "123465", 1000 ) );
        assert ( x1 . NewAccount ( "123466", 1000 ) );
        assert ( x1 . NewAccount ( "123467", 1000 ) );
        assert ( x1 . NewAccount ( "123468", 1000 ) );
        assert ( x1 . NewAccount ( "123469", 1000 ) );
        assert ( x1 . NewAccount ( "123470", 1000 ) );
        assert ( x1 . NewAccount ( "123471", 1000 ) );
        assert ( x1 . NewAccount ( "123472", 1000 ) );
        assert ( x1 . NewAccount ( "123473", 1000 ) );
        assert ( x1 . NewAccount ( "123474", 1000 ) );
        assert ( x1 . NewAccount ( "123475", 1000 ) );
        assert ( x1 . NewAccount ( "123476", 1000 ) );
        assert ( x1 . NewAccount ( "123477", 1000 ) );
        assert ( x1 . NewAccount ( "123478", 1000 ) );
        assert ( x1 . NewAccount ( "123479", 1000 ) );
        assert ( x1 . NewAccount ( "123480", 1000 ) );
        assert ( x1 . NewAccount ( "123481", 1000 ) );
        assert ( x1 . NewAccount ( "123482", 1000 ) );
        assert ( x1 . NewAccount ( "123483", 1000 ) );
        assert ( x1 . NewAccount ( "123484", 1000 ) );
        assert ( x1 . NewAccount ( "123485", 1000 ) );
        assert ( x1 . NewAccount ( "123486", 1000 ) );
        assert ( x1 . NewAccount ( "123487", 1000 ) );
        assert ( x1 . NewAccount ( "123488", 1000 ) );
        assert ( x1 . NewAccount ( "123489", 1000 ) );
        assert ( x1 . NewAccount ( "123490", 1000 ) );
        assert ( x1 . NewAccount ( "123491", 1000 ) );
        assert ( x1 . NewAccount ( "123492", 1000 ) );
        assert ( x1 . NewAccount ( "123493", 1000 ) );
        assert ( x1 . NewAccount ( "123494", 1000 ) );
        assert ( x1 . NewAccount ( "123495", 1000 ) );
        assert ( x1 . NewAccount ( "123496", 1000 ) );
        assert ( x1 . NewAccount ( "123497", 1000 ) );
        assert ( x1 . NewAccount ( "123498", 1000 ) );
        assert ( x1 . NewAccount ( "123499", 1000 ) );
        assert ( x1 . NewAccount ( "123500", 1000 ) );
        assert ( x1 . NewAccount ( "123501", 1000 ) );
        assert ( x1 . NewAccount ( "123502", 1000 ) );
        assert ( x1 . NewAccount ( "123503", 1000 ) );
        assert ( x1 . NewAccount ( "123504", 1000 ) );
        assert ( x1 . NewAccount ( "123505", 1000 ) );
        assert ( x1 . NewAccount ( "123506", 1000 ) );
        assert ( x1 . NewAccount ( "123507", 1000 ) );
        assert ( x1 . NewAccount ( "123508", 1000 ) );
        assert ( x1 . NewAccount ( "123509", 1000 ) );
        assert ( x1 . NewAccount ( "123510", 1000 ) );
        assert ( x1 . NewAccount ( "123511", 1000 ) );
        assert ( x1 . NewAccount ( "123512", 1000 ) );
        assert ( x1 . NewAccount ( "123513", 1000 ) );
        assert ( x1 . NewAccount ( "123514", 1000 ) );
        assert ( x1 . NewAccount ( "123515", 1000 ) );
        assert ( x1 . NewAccount ( "123516", 1000 ) );
        assert ( x1 . NewAccount ( "123517", 1000 ) );
        assert ( x1 . NewAccount ( "123518", 1000 ) );
        assert ( x1 . NewAccount ( "123519", 1000 ) );
        assert ( x1 . NewAccount ( "123520", 1000 ) );
        assert ( x1 . NewAccount ( "123521", 1000 ) );
        assert ( x1 . NewAccount ( "123522", 1000 ) );
        assert ( x1 . NewAccount ( "123523", 1000 ) );
        assert ( x1 . NewAccount ( "123524", 1000 ) );
        assert ( x1 . NewAccount ( "123525", 1000 ) );
        assert ( x1 . NewAccount ( "123526", 1000 ) );
        assert ( x1 . NewAccount ( "123527", 1000 ) );
        assert ( x1 . NewAccount ( "123528", 1000 ) );
        assert ( x1 . NewAccount ( "123529", 1000 ) );
        assert ( x1 . NewAccount ( "123530", 1000 ) );
        assert ( x1 . NewAccount ( "123531", 1000 ) );
        assert ( x1 . NewAccount ( "123532", 1000 ) );
        assert ( x1 . NewAccount ( "123533", 1000 ) );
        assert ( x1 . NewAccount ( "123534", 1000 ) );
        assert ( x1 . NewAccount ( "123535", 1000 ) );
        assert ( x1 . NewAccount ( "123536", 1000 ) );
        assert ( x1 . NewAccount ( "123537", 1000 ) );
        assert ( x1 . NewAccount ( "123538", 1000 ) );
        assert ( x1 . NewAccount ( "123539", 1000 ) );
        assert ( x1 . NewAccount ( "123540", 1000 ) );
        assert ( x1 . NewAccount ( "123541", 1000 ) );
        assert ( x1 . NewAccount ( "123542", 1000 ) );
        assert ( x1 . NewAccount ( "123543", 1000 ) );
        assert ( x1 . NewAccount ( "123544", 1000 ) );
        assert ( x1 . NewAccount ( "123545", 1000 ) );
        assert ( x1 . NewAccount ( "123546", 1000 ) );
        assert ( x1 . NewAccount ( "123547", 1000 ) );
        assert ( x1 . NewAccount ( "123548", 1000 ) );
        assert ( x1 . NewAccount ( "123549", 1000 ) );
        assert ( x1 . NewAccount ( "123550", 1000 ) );
        assert ( x1 . NewAccount ( "123551", 1000 ) );
        assert ( x1 . NewAccount ( "123552", 1000 ) );
        assert ( x1 . NewAccount ( "123553", 1000 ) );
        assert ( x1 . NewAccount ( "123554", 1000 ) );
        assert ( x1 . NewAccount ( "123555", 1000 ) );
        assert ( x1 . NewAccount ( "123556", 1000 ) );
        assert ( x1 . NewAccount ( "123557", 1000 ) );
        assert ( x1 . NewAccount ( "123558", 1000 ) );
        assert ( x1 . NewAccount ( "123559", 1000 ) );
        assert ( x1 . NewAccount ( "123560", 1000 ) );
        assert ( x1 . NewAccount ( "123561", 1000 ) );
        assert ( x1 . NewAccount ( "123562", 1000 ) );
        assert ( x1 . NewAccount ( "123563", 1000 ) );
        assert ( x1 . NewAccount ( "123564", 1000 ) );
        assert ( x1 . NewAccount ( "123565", 1000 ) );
        assert ( x1 . NewAccount ( "123566", 1000 ) );
        assert ( x1 . NewAccount ( "123567", 1000 ) );
        assert ( x1 . NewAccount ( "123568", 1000 ) );
        assert ( x1 . NewAccount ( "123569", 1000 ) );
        assert ( x1 . NewAccount ( "123570", 1000 ) );
        assert ( x1 . NewAccount ( "123571", 1000 ) );
        assert ( x1 . NewAccount ( "123572", 1000 ) );
        assert ( x1 . NewAccount ( "123573", 1000 ) );
        assert ( x1 . NewAccount ( "123574", 1000 ) );

        assert ( x1 . Transaction ( "123456", "123457", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123458", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123459", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123460", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123461", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123462", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123463", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123464", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123465", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123466", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123467", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123468", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123469", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123470", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123471", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123472", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123473", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123474", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123475", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123476", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123477", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123478", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123479", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123480", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123481", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123482", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123483", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123484", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123485", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123486", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123487", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123488", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123489", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123490", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123491", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123492", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123493", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123494", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123495", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123496", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123497", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123498", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123499", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123500", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123501", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123502", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123503", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123504", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123505", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123506", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123507", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123508", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123509", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123510", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123511", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123512", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123513", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123514", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123515", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123516", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123517", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123518", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123519", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123520", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123521", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123522", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123523", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123524", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123525", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123526", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123527", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123528", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123529", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123530", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123531", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123532", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123533", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123534", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123535", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123536", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123537", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123538", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123539", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123540", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123541", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123542", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123543", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123544", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123545", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123546", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123547", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123548", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123549", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123550", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123551", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123552", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123553", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123554", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123555", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123556", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123557", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123558", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123559", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123560", 300, "XAbG5uKz6E=" ) );
        assert ( x1 . Transaction ( "123456", "123561", 300, "XAbG5uKz6E=" ) );

        CBank x4;

        assert ( !x4.Transaction( "" , "1" , 10, "aslejf" ) );
        assert ( !x4.Transaction( "" , "1" , 10, "aslejf" ) );
        assert ( !x4.Transaction( "" , "1" , 10, "aslejf" ) );
    }
//------------------- test na destruktor - hodne krat CBank promena bez niceho --------------------------------------
    {
        CBank x199;
        CBank x13;
        CBank x11;
        CBank x5;
        CBank x3;
        CBank x2(x3);

        x199 = x2;
        x199 = x199;

        assert ( !x2.Transaction( "1" , "2" , 100, "jsalefjsl" ) );
    }

    
//--------------------------- ukazkove testy ----------------------------------
    {
                
        char accCpy[100], debCpy[100], credCpy[100], signCpy[100];
        
        


        CBank x2;
        strncpy ( accCpy, "123456", sizeof ( accCpy ) );
        assert ( x2 . NewAccount ( accCpy, 1000 ));
        strncpy ( accCpy, "987654", sizeof ( accCpy ) );
        assert ( x2 . NewAccount ( accCpy, -500 ));
        strncpy ( debCpy, "123456", sizeof ( debCpy ) );
        strncpy ( credCpy, "987654", sizeof ( credCpy ) );
        strncpy ( signCpy, "XAbG5uKz6E=", sizeof ( signCpy ) );
        assert ( x2 . Transaction ( debCpy, credCpy, 300, signCpy ) );
        strncpy ( debCpy, "123456", sizeof ( debCpy ) );
        strncpy ( credCpy, "987654", sizeof ( credCpy ) );
        strncpy ( signCpy, "AbG5uKz6E=", sizeof ( signCpy ) );
        assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) );
        strncpy ( accCpy, "111111", sizeof ( accCpy ) );
        assert ( x2 . NewAccount ( accCpy, 5000 ));
        strncpy ( debCpy, "111111", sizeof ( debCpy ) );
        strncpy ( credCpy, "987654", sizeof ( credCpy ) );
        strncpy ( signCpy, "Okh6e+8rAiuT5=", sizeof ( signCpy ) );
        assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) );
        assert ( x2 . Account ( "123456" ). Balance ( ) ==  -2190 );
        assert ( x2 . Account ( "987654" ). Balance ( ) ==  5580 );
        assert ( x2 . Account ( "111111" ). Balance ( ) ==  2110 );
        os . str ( "" );
        os << x2 . Account ( "123456" );
        assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) );
        os . str ( "" );
        os << x2 . Account ( "987654" );
        assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n = 5580\n" ) );
        os . str ( "" );
        os << x2 . Account ( "111111" );
        assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n = 2110\n" ) );
        assert ( x2 . TrimAccount ( "987654" ) );
        strncpy ( debCpy, "111111", sizeof ( debCpy ) );
        strncpy ( credCpy, "987654", sizeof ( credCpy ) );
        strncpy ( signCpy, "asdf78wrnASDT3W", sizeof ( signCpy ) );
        assert ( x2 . Transaction ( debCpy, credCpy, 123, signCpy ) );
        os . str ( "" );
        os << x2 . Account ( "987654" );
        assert ( ! strcmp ( os . str () . c_str (), "987654:\n   5580\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );

        CBank x4;
        assert ( x4 . NewAccount ( "123456", 1000 ) );
        assert ( x4 . NewAccount ( "987654", -500 ) );
        assert ( !x4 . NewAccount ( "123456", 3000 ) );
        assert ( !x4 . Transaction ( "123456", "666", 100, "123nr6dfqkwbv5" ) );
        assert ( !x4 . Transaction ( "666", "123456", 100, "34dGD74JsdfKGH" ) );
        assert ( !x4 . Transaction ( "123456", "123456", 100, "Juaw7Jasdkjb5" ) );
        try
        {
            x4 . Account ( "666" ). Balance ( );
            assert ( "Missing exception !!" == NULL );
        }
        catch ( ... )
        {
        }
        try
        {
            os << x4 . Account ( "666" ). Balance ( );
            assert ( "Missing exception !!" == NULL );
        }
        catch ( ... )
        {
        }
        assert ( !x4 . TrimAccount ( "666" ) );

        CBank x6;
        assert ( x6 . NewAccount ( "123456", 1000 ) );
        assert ( x6 . NewAccount ( "987654", -500 ) );
        assert ( x6 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
        assert ( x6 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
        assert ( x6 . NewAccount ( "111111", 5000 ) );
        assert ( x6 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
        CBank x7 ( x6 );
        
        assert ( x6 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
        assert ( x7 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
        assert ( x6 . NewAccount ( "99999999", 7000 ) );
        assert ( x6 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
        assert ( x6 . TrimAccount ( "111111" ) );
        assert ( x6 . Transaction ( "123456", "111111", 221, "Q23wr234ER==" ) );
        os . str ( "" );
        os << x6 . Account ( "111111" );
        assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n + 221, from: 123456, sign: Q23wr234ER==\n = -1581\n" ) );
        
        os . str ( "" );
        os << x6 . Account ( "99999999" );
        assert ( ! strcmp ( os . str () . c_str (), "99999999:\n   7000\n + 3789, from: 111111, sign: aher5asdVsAD\n = 10789\n" ) );
        os . str ( "" );
        os << x6 . Account ( "987654" );
        assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );
        os . str ( "" );
        os << x7 . Account ( "111111" );
        assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );
        try
        {
            os << x7 . Account ( "99999999" ). Balance ( );
            assert ( "Missing exception !!" == NULL );
        }
        catch ( ... )
        {
        }
        os . str ( "" );
        os << x7 . Account ( "987654" );
        assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 789, from: 111111, sign: SGDFTYE3sdfsd3W\n = 6369\n" ) );

        CBank x8;
        CBank x9;
        assert ( x8 . NewAccount ( "123456", 1000 ) );
        assert ( x8 . NewAccount ( "987654", -500 ) );
        assert ( x8 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
        assert ( x8 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
        assert ( x8 . NewAccount ( "111111", 5000 ) );
        assert ( x8 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
        x9 = x8;
        assert ( x8 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
        assert ( x9 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
        assert ( x8 . NewAccount ( "99999999", 7000 ) );
        assert ( x8 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
        assert ( x8 . TrimAccount ( "111111" ) );
        os . str ( "" );
        os << x8 . Account ( "111111" );
        assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n = -1802\n" ) );
        os . str ( "" );
        os << x9 . Account ( "111111" );
        assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );

    }
//----------------------------- random ass test ------------------------------------
    {
        CBank x1;
        assert( x1.NewAccount( "acc1" , 100 ) );
        assert( x1.NewAccount( "acc2" , 1000 ) );
        assert( x1.NewAccount( "acc3" , 150 ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig2" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 1100 , "si1" ) );
    }

//--------------------------- random ass ----------------------------------
    {
        CBank x1;
        assert( x1.NewAccount( "acc1" , 100 ) );
        assert( x1.NewAccount( "acc2" , 1000 ) );
        assert( x1.NewAccount( "acc3" , 150 ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig2" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 1100 , "si1" ) );
        CBank x2( x1 );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig2" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 1100 , "si1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig2" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 1100 , "si1" ) );
        assert( x2.TrimAccount( "acc1" ) );
        assert( x2.TrimAccount( "acc2" ) );
        assert( x2.TrimAccount( "acc3" ) );
        CBank x3( x2 );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig2" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 1100 , "si1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig2" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 1100 , "si1" ) );
        x1 = x3;
        x2 = x1;
        x3 = x2;
        os << x1.Account( "acc1" );
        CBank x4;
        x1 = x4;
        x2 = x4;
        x3 = x4;
        assert( !x1.Transaction( "acc1" , "acc3" , 100 , "sig1" ) );
        assert( !x1.Transaction( "acc1" , "acc3" , 100 , "sig2" ) );
        assert( !x1.Transaction( "acc1" , "acc3" , 1100 , "si1" ) );
        assert( x1.NewAccount( "acc1" , 100 ) );
        assert( x1.NewAccount( "acc2" , 1000 ) );
        assert( x1.NewAccount( "acc3" , 150 ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig1" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 100 , "sig2" ) );
        assert( x1.Transaction( "acc1" , "acc3" , 1100 , "si1" ) );

    }


//------------------------- test metod s prazdnym polem -------------------------------------------------------------
    {
        CBank x1;
        assert( !x1.Transaction( "1", "2", 10, "aslfje" ) );
        assert( !x1.Transaction( "1", "1", 10, "aslfje" ) );
        assert( !x1.Transaction( "1", "123", 2343210, "aslfje" ) );
        assert( !x1.TrimAccount( "123" ) );
        CBank x2 = x1;
        assert( !x2.Transaction( "1", "2", 10, "aslfje" ) );
        assert( !x2.Transaction( "1", "1", 10, "aslfje" ) );
        assert( !x2.Transaction( "1", "123", 2343210, "aslfje" ) );
        assert( !x2.TrimAccount( "123" ) );
    }

//------------------------ test metod when only 1 a 2 acc is there ---------------------------------------------------------
    {
        CBank x1;
        assert( x1.NewAccount( "acc1", 100 ) );
        assert( !x1.Transaction( "asfe", "fslife", 10, "sleifj" ) );
        assert( !x1.Transaction( "1", "acc1", 234, "alsiefsifj" ) );
        assert( x1.NewAccount( "acc2" , 100 ) );
        assert( !x1.Transaction( "acc1", "1", 234, "alsiefsifasefj" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.Transaction( "acc2" , "acc1" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc2" , "acc1" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc2" , "acc1" , 100, "asliefsifj" ) );
        CBank k1 = x1;
        CBank g1;
        x1 = g1;
        assert( !x1.Transaction( "acc2" , "acc1" , 100, "asliefsifj" ) );
        assert( !x1.Transaction( "acc2" , "acc1" , 100, "asliefsifj" ) );
        assert( !x1.Transaction( "acc2" , "acc1" , 100, "asliefsifj" ) );
        assert( !x1.Transaction( "acc2" , "acc1" , 100, "asliefsifj" ) );
        assert( !x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( !x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( !x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( !x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        x1 = k1;
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.NewAccount( "acc" , 100 ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.NewAccount( "a234cc" , 100 ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.NewAccount( "ac2342c" , 100 ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        assert( x1.TrimAccount( "acc1" ) );
        assert( x1.Transaction( "acc1" , "acc2" , 100, "asliefsifj" ) );
        os << x1.Account("acc1");
        os << x1.Account("acc1").Balance();
        os << x1.Account("acc1");
        os << x1.Account("acc1").Balance();
        os << x1.Account("acc1");
        os << x1.Account("acc1").Balance();
        os << x1.Account("acc1");
        os << x1.Account("acc1").Balance();
        os << x1.Account("acc1");
        os << x1.Account("acc1").Balance();
        CBank x2;
        x1 = x2;
        assert( !x1.TrimAccount( "acc1" ) );

    }

    cout << "---------------------------- done --------------------------------------" << endl;
    cout << "---------------------------- done --------------------------------------" << endl;
    cout << "---------------------------- done --------------------------------------" << endl;
    cout << "---------------------------- done --------------------------------------" << endl;

    return 0;
}
#endif /* __PROGTEST__ */
