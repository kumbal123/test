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
  private:
    int m_Amount;
    char * m_OtherID;
    char * m_Sign;
    char m_Mark;
  public:
    CTransaction(): m_Amount( 0 ),
                    m_OtherID( nullptr ),
                    m_Sign( nullptr ){}
    
    CTransaction( int amount, const char * OtherID, const char * sign, char mark ):
                    m_Amount( amount ),
                    m_OtherID( new char[ strlen( OtherID ) + 1 ] ),
                    m_Sign( new char[ strlen( sign ) + 1 ] ),
                    m_Mark( mark )
    {
        memcpy( m_OtherID, OtherID, strlen( OtherID ) + 1 );
        memcpy( m_Sign, sign, strlen( sign ) + 1 );
    }

    void deleteCTransaction(){
        delete [] m_OtherID;
        delete [] m_Sign;
    }

    int getAmount(){
        return m_Amount;
    }

    const char * getOtherID(){
        return m_OtherID;
    }

    const char * getSign(){
        return m_Sign;
    }

    char getMark(){
        return m_Mark;
    }
};
// ------------------------------------- CAcc class --------------------------------------------------------------
class CAcc{
  private:
    char * m_AccID;
    int m_InitialBalance;
    int m_Balance;
    int m_SizeTrans;
    int m_MaxTrans;
    CTransaction * m_Transaction;
  public:
    CAcc(): m_AccID( nullptr ),
            m_InitialBalance( 0 ),
            m_Balance( 0 ),
            m_SizeTrans( 0 ),
            m_MaxTrans( 0 ),
            m_Transaction( nullptr ){}

    CAcc( const char * accID, int initialBalance ):
            m_AccID( new char[ strlen( accID ) + 1 ] ),
            m_InitialBalance( initialBalance ),
            m_Balance( initialBalance ),
            m_SizeTrans( 0 ),
            m_MaxTrans( 0 ),
            m_Transaction( nullptr )
    {
        memcpy( m_AccID, accID, strlen( accID ) + 1 );
    }

    void deleteCAcc(){
        delete [] m_AccID;
        for( int i = 0; i < m_SizeTrans; ++i ){
            m_Transaction[i].deleteCTransaction();
        }
        delete [] m_Transaction;
    }

    void TrimAcc(){
        for( int i = 0; i < m_SizeTrans; ++i ){
            m_Transaction[i].deleteCTransaction();
        }
        delete [] m_Transaction;
        m_Transaction = nullptr;
        m_SizeTrans = 0;
        m_MaxTrans = 0;
        m_InitialBalance = m_Balance;
    }

    const char * getAccID(){
        return m_AccID;
    }

    int getInitialBalance(){
        return m_InitialBalance;
    }

    int getBalance(){
        return m_Balance;
    }

    int getSizeTrans(){
        return m_SizeTrans;
    }

    int getMaxTrans(){
        return m_MaxTrans;
    }

    CTransaction * getTransaction(){
        return m_Transaction;
    }

    CTransaction getTransaction( int index ){
        return m_Transaction[ index ];
    }

    void setTransaction( CTransaction * x ){
        m_Transaction = x;
    }

    void newInitialBalance(){
        m_InitialBalance = m_Balance;
    }

    void setBalance( int balance ){
        m_Balance = balance;
    }

    void setSize( int size ){
        m_SizeTrans = size;
    }

    void setMax( int max ){
        m_MaxTrans = max;
    }

    int Balance(){
        return m_Balance;
    }

    void addBalance( int amount ){
        m_Balance += amount;
    }

    void realloc(){
        if( m_SizeTrans == m_MaxTrans ){
            m_MaxTrans += m_MaxTrans < 100 ? 10 : m_MaxTrans/2;
            CTransaction * tmpTrans = new CTransaction[ m_MaxTrans ];
            for( int j = 0; j < m_SizeTrans; ++j ){
                CTransaction tmp2 = CTransaction( m_Transaction[ j ].getAmount(),
                                                  m_Transaction[ j ].getOtherID(),
                                                  m_Transaction[ j ].getSign(),
                                                  m_Transaction[ j ].getMark() );
                tmpTrans[j] = tmp2;
                delete [] m_Transaction[j].getOtherID();
                delete [] m_Transaction[j].getSign();
            }
            delete [] m_Transaction;
            m_Transaction = tmpTrans;
        }
    }

    void NewTransaction( int amount, const char * OtherID, const char * sign, char mark ){
        this -> realloc();
        m_Transaction[ m_SizeTrans ] = CTransaction( amount, OtherID, sign, mark );
        m_SizeTrans++;
    }
};
// ------------------------------------- CBridge class ----------------------------------------------------------------------
class CBridge{
  private:
    CAcc * m_Accounts;
    int m_SizeAcc;
    int m_MaxAcc;
    unsigned int m_SumPointer;
  public:
    // default constructor
    CBridge(): m_Accounts( nullptr ),
             m_SizeAcc( 0 ),
             m_MaxAcc( 0 ),
             m_SumPointer( 1 ){}
    // copy constructor
    CBridge( CBridge & x ): m_Accounts( x.newCopy() ),
                        m_SizeAcc( x.m_SizeAcc ),
                        m_MaxAcc( x.m_MaxAcc ),
                        m_SumPointer( 1 ){}
    // destructor
    void deleteBridge(){
        for( int i = 0; i < m_SizeAcc; ++i ){
            m_Accounts[i].deleteCAcc();
        }
        delete [] m_Accounts;
    }
    void realloc(){
        if( m_SizeAcc == m_MaxAcc ){
            m_MaxAcc += m_MaxAcc < 100 ? 10 : m_MaxAcc/2;
            CAcc * tmp = new CAcc[ m_MaxAcc ];
            copy( m_Accounts, m_Accounts + m_SizeAcc, tmp );
            delete [] m_Accounts;
            m_Accounts = tmp;
        }
    }
    CAcc * newCopy(){
        CAcc * tmpAcc = new CAcc[ m_MaxAcc ];
        for( int i = 0; i < m_SizeAcc; ++i ){
            CAcc tmp1 = CAcc( m_Accounts[i].getAccID(), m_Accounts[i].getInitialBalance() );
            tmp1.setBalance( m_Accounts[i].getBalance() );
            tmp1.setSize( m_Accounts[i].getSizeTrans() );
            tmp1.setMax( m_Accounts[i].getMaxTrans() );
            CTransaction * tmpTrans = new CTransaction[ m_Accounts[i].getMaxTrans() ];
            for( int j = 0; j < m_Accounts[i].getSizeTrans(); ++j ){
                CTransaction tmp2 = CTransaction( m_Accounts[i].getTransaction( j ).getAmount(),
                                                  m_Accounts[i].getTransaction( j ).getOtherID(),
                                                  m_Accounts[i].getTransaction( j ).getSign(),
                                                  m_Accounts[i].getTransaction( j ).getMark() );
                tmpTrans[j] = tmp2;
            }
            tmp1.setTransaction( tmpTrans );
            tmpAcc[i] = tmp1;
        }
        return tmpAcc;
    }
    // operator =
    void operator = ( CBridge & x );

    bool   NewAccount    ( const char * accID, int initialBalance );
    bool   Transaction   ( const char * debAccID, const char * credAccID,
                           unsigned int amount, const char * signature );
    bool   TrimAccount   ( const char * accID );
    // Account ( accID )
    CAcc & Account ( const char * accID );  
    friend ostream & getStream ( ostream & out, CAcc & x );

    void addPointer(){
        m_SumPointer++;
    }

    void removePointer(){
        m_SumPointer--;
    }

    void setSizeAcc( int size ){
        m_SizeAcc = size;
    }

    void setMaxAcc( int max ){
        m_MaxAcc = max;
    }

    CAcc * getAccounts(){
        return m_Accounts;
    }

    unsigned int getPointer(){
        return m_SumPointer;
    }
};

void CBridge::operator = ( CBridge & x ){
    this -> ~CBridge();
    m_SizeAcc = x.m_SizeAcc;
    m_MaxAcc = x.m_MaxAcc;
    m_Accounts = x.newCopy();
}

bool CBridge::NewAccount ( const char * accID, int initialBalance ){
    for( int i = 0; i < m_SizeAcc; i++ ){
        if( !strcmp( accID, m_Accounts[i].getAccID() ) )
            return false;
    }

    realloc();
    m_Accounts[ m_SizeAcc ] = CAcc( accID, initialBalance );
    m_SizeAcc++;
    return true;
}

bool   CBridge::Transaction   ( const char * debAccID, const char * credAccID,
                               unsigned int amount, const char * signature ){
    if( !strcmp( debAccID, credAccID ) )
        return false;
    int debAccIndex = -1, credAccIndex = -1;
    for( int i = 0; i < m_SizeAcc; i++ ){
        if( !strcmp( debAccID, m_Accounts[i].getAccID() ) )
            debAccIndex = i;
        else if( !strcmp( credAccID, m_Accounts[i].getAccID() ) )
            credAccIndex = i;
    }

    if( credAccIndex == -1 || debAccIndex == -1 )
        return false;

    m_Accounts[ debAccIndex ].addBalance( -1 * amount );
    m_Accounts[ credAccIndex ].addBalance( amount );

    m_Accounts[ debAccIndex ].NewTransaction( amount, credAccID, signature, '-' );
    m_Accounts[ credAccIndex ].NewTransaction( amount, debAccID, signature, '+' );
    return true;
}

bool   CBridge::TrimAccount   ( const char * accID ){
    int accIDIndex = -1;
    for( int i = 0; i < m_SizeAcc; i++ ){
        if( !strcmp( accID, m_Accounts[i].getAccID() ) ){
            accIDIndex = i;
            break;
        }
    }

    if( accIDIndex == -1 )
        return false;

    m_Accounts[ accIDIndex ].TrimAcc();
    return true;
}

CAcc & CBridge::Account ( const char * accID ){
    int accIDIndex = -1;

    for( int i = 0; i < m_SizeAcc; i++ ){
        if( !strcmp( accID, m_Accounts[i].getAccID() ) ){
            accIDIndex = i;
            break;
        }
    }

    if( accIDIndex == -1 )
        throw runtime_error( "no accID found " );
    
    return m_Accounts[ accIDIndex ];
}

ostream & getSTream ( ostream & out, CAcc & x ){
    out << x.getAccID() << ":\n   " << x.getInitialBalance() << "\n";
    for( int i = 0; i < x.getSizeTrans(); ++i ){
        out << " " << x.getTransaction( i ).getMark() << " " << x.getTransaction( i ).getAmount() << ", ";
        if( x.getTransaction( i ).getMark() == '+' )
            out << "from: ";
        else
            out << "to: ";
        out << x.getTransaction( i ).getOtherID() << ", sign: " << x.getTransaction( i ).getSign() << "\n";
    }
    out << " = " << x.getBalance() << "\n";
    return out;
}

class CBank{
  private:
    CBridge * m_Bridge;
  public:
    CBank(): m_Bridge( new CBridge() ){}
    CBank( CBank & x ): m_Bridge( x.m_Bridge )
    {
        x.m_Bridge -> addPointer();
    }

    // destructor
    ~CBank(){
        if( m_Bridge -> getPointer() > 1 ){
            m_Bridge -> removePointer();
            m_Bridge = nullptr;
        }else if( m_Bridge -> getPointer() == 1 ){
            m_Bridge -> removePointer();
            m_Bridge -> deleteBridge();
            delete m_Bridge;
        }
    }
    // operator =
    void operator = ( CBank & x ){
        this -> ~CBank();
        if( x.m_Bridge )
            x.m_Bridge -> addPointer();
        m_Bridge = x.m_Bridge;
    }

    bool   NewAccount    ( const char * accID,
                           int          initialBalance );
    bool   Transaction   ( const char * debAccID,
                           const char * credAccID,
                           unsigned int amount,
                           const char * signature );
    bool   TrimAccount   ( const char * accID );
    CAcc & Account( const char * accID );
    friend ostream & operator << ( ostream & out, CAcc & x );
};

bool   CBank::NewAccount    ( const char * accID, int initialBalance ){
    if( !m_Bridge )
        return false;
    if( m_Bridge -> getPointer() > 1 ){
        m_Bridge -> removePointer();
        m_Bridge = new CBridge( *m_Bridge );
    }

    if( !m_Bridge -> NewAccount( accID, initialBalance ) )
        return false;
    return true;
}

bool   CBank::Transaction   ( const char * debAccID,
                        const char * credAccID,
                        unsigned int amount,
                        const char * signature ){
    if( !m_Bridge )
        return false;
    if( m_Bridge -> getPointer() > 1 ){
        m_Bridge -> removePointer();
        m_Bridge = new CBridge( *m_Bridge );
    }
    if( !m_Bridge -> Transaction( debAccID, credAccID, amount, signature ) )
        return false;
    return true;
}

bool   CBank::TrimAccount   ( const char * accID ){
    if( !m_Bridge )
        return false;
    if( m_Bridge -> getPointer() > 1 ){
        m_Bridge -> removePointer();
        m_Bridge = new CBridge( *m_Bridge );
    }
    if( !m_Bridge -> TrimAccount( accID ) )
        return false;
    return true;
}

CAcc & CBank::Account( const char * accID ){
    return m_Bridge -> Account( accID );
}

ostream & operator << ( ostream & out, CAcc & x ){
    return getSTream( out, x );
}



//------------------------------------------------------ main -------------------------------------------------------------------
#ifndef __PROGTEST__
int main ( void )
{
        int x;
        x = 5 + 6;

        cout << "x: " << x << endl;

  
    cout << "--------------------------------------- done ---------------------------------------------" << endl;
    cout << "--------------------------------------- done ---------------------------------------------" << endl;
    cout << "--------------------------------------- done ---------------------------------------------" << endl;
  return 0;
}
#endif /* __PROGTEST__ */
