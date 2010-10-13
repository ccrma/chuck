// 30.ck : more test

Object x[10];
x[9].testID( 3 );
<<<x[9].testID()>>>;
<<<x[9].m_testID>>>;

if( x[9].m_testID == 3 )
    <<<"success">>>;
