// 20180426 16:03分 侯昱宏 開始拓荒:<
//  cin >> ID            cout << expression
//  vector<string> * t
//  t = new vector<string>()
//  (int) 部分檢查

# include <iostream>
//  # include <fstream>
# include <cstring>
# include <string> 
# include <vector>
# include <cstdio>
# include <cstdlib>
// # include <ctime>
# include <iomanip>
// # include <algorithm>
# include <stdexcept> //  std::out_of_range
# include <cmath>
// # include <set>
# include <map>
# include  <iomanip>
// # include  <typeinfo>
# include  <sstream>
# include  <cctype>
// # include  <bitset>
# include <exception>
# include <string.h>

# define NOT !

using namespace std;


int gLine = 1;
int gpreLine = 0; // 被誤讀掉的行數
int gpos = 0; // 紀錄變數stack 位置
int gexe_pos = 0;  // exe階段 不用gettoken 直接去正確token vector中取 此變數為記錄取到哪用

bool g_control = false; // 嗚嗚 為了解決 peektoken 行數問題才加的一個bool
bool gpos_no_use = false;
bool gexe = false; // 執行階段
bool gis_cout = false ;
bool gexe_jump = false ;
bool ghave_error = false ;

struct Information
{
  string variable_name; // 這個變數名稱
  string variable_type; // 這個變數的型別

  string data[500]; // 可能是 string  需要存字串
  string assign_type; // += = -= *=....哪一種
  string pp_mm; // 是 PP還是MM
  string sign; // + - !



  double value[500];  // 值
  bool isFloat;
  bool boolenop; // 是否是 true or false 
  bool line_enter;
  bool isFunc; // 此次文法是不是function宣告



  vector<string> name; // 宣告時可能會有不只一個 例如 int a,b,c

  vector<string> token; // 存放此次文法的所有token


};



class Exception {
public:
  string mError_Message;
  Exception() {  }

  Exception( string x ) {
    mError_Message = x;
  } // Exception()

}; //  class Expception


class  LexicalError : public Exception {

public:


  LexicalError( string token ) {

    stringstream ss;
    ss << gLine;
    string line = ss.str();
    mError_Message = "Line " + line + " : " + mError_Message +
      "unrecognized token with first char '" + token + "'\n"  ; 

  } // LexicalError()

  LexicalError( char token ) {
    stringstream ss;
    ss << gLine;
    string line = ss.str();
    mError_Message = "Line " + line + " : " + mError_Message +
      "unrecognized token with first char '" + token + "'\n" ;

  } // LexicalError()


};

class  SyntacticError : public Exception {

public:


  SyntacticError( string token ) {

    stringstream ss;
    ss << gLine;
    string line = ss.str();
    mError_Message = "Line " + line + " : " + mError_Message +
      "unexpected token '" + token + "'\n" ;

  } // SyntacticError()

  SyntacticError( char  token ) {

    stringstream ss;
    ss << gLine;
    string line = ss.str();
    mError_Message = "Line " + line + " : " + mError_Message +
      "unexpected token '" + token + "'\n" ;

  } // SyntacticError()


};



class  Divid_Zero : public Exception {

public:


  Divid_Zero() {
    mError_Message = "Error\n";

  } // Divid_Zero()



};

class  End : public Exception {

public:


  End() {
    mError_Message = "Our-C exited ...";

  } // End()



};


class  Undefined_identifier : public Exception {

public:


  Undefined_identifier( string token ) {

    stringstream ss;
    ss << gLine;
    string line = ss.str();
    mError_Message = "Line " + line + " : " + mError_Message +
      "undefined identifier '" + token + "'\n" ;

  } // Undefined_identifier()

  Undefined_identifier( char token ) {

    stringstream ss;
    ss << gLine;
    string line = ss.str();
    mError_Message = "Line " + line + " : " + mError_Message +
      "undefined identifier '" + token + "'\n" ;

  } // Undefined_identifier()


};


class Data
{
private:


  int mpos; // 輔助 VeriableList pop時要停止的位置


public:

  vector<string> * mCorrect_Token;  // 存放跑完文法後 確定正確的Token
  vector<Information> * mVariableList; // 用來存放宣告過的變數(當前區域才看得到)
  map<string, vector<string> > mInformation_table;  // 用來在ListFunction && ListVariable 使用


  Data() // 預設 constructor
  {
    mCorrect_Token = new vector<string>();  // 存放跑完文法後 確定正確的Token
    mVariableList = new vector<Information>(); // 用來存放宣告過的變數(當前區域才看得到)
  } // Data()

  void Remove_White_Space() //  每次GetToken時都要呼叫，確保要GetToken時是某個字的開頭
  {
    while ( cin.peek() == ' ' || cin.peek() == '\t' || cin.peek() == '\n' ) //  跳過 空格 TAB 和 換行
    {
      if ( cin.peek() == '\n' )
        gLine++;

      cin.get();  //  只讀一個字元
    } // while

  } // Remove_White_Space()

  void Remove_One_Line_White_Space()
  {
    bool run = true;
    char temp[128];
    while ( ( cin.peek() == ' ' || cin.peek() == '\t' || cin.peek() == '\n' || cin.peek() == '/' ) && run )
    { //  跳過 空格 TAB 和 換行
      if ( cin.peek() == '\n' )
        run = false;



      if ( cin.peek() == '/' )
      {

        cin.get();
        if ( cin.peek() != '/' )
          cin.putback( '/' );
        else  // 註解 讀掉後面 
        {
          cin.getline( temp, 128 );
        } // else

        run = false;

      } // if

      else
        cin.get();  //  只讀一個字元

    } // while

  } // Remove_One_Line_White_Space()

  void Vector_pop_to_original( int pre )
  {
    while ( mVariableList->size() > pre )
    {
      mVariableList->pop_back();
    } // while


  } // Vector_pop_to_original()

  bool Is_Letter( char c ) //  用來判斷此字元是不是字母
  {
    return isalpha( c );

  } // Is_Letter()

  bool Is_Letter( string s )
  {
    if ( isalpha( s.at( 0 ) ) )
      return true;
    else
      return false;


  } // Is_Letter()

  bool Is_Num_NoDot( string str )  //  判斷是否為純數字 而沒有小數點 若有則回傳false
  {
    for ( int i = 0; i < str.size() ; i++ )
    {
      if ( str.at( i ) == '.' )
        return false;

    } // for

    return true;

  } // Is_Num_NoDot()

  bool Is_Num( char c ) //  判斷單一字元是否為數字  可檢查 .5 此情況
  {
    char c_temp = '\0';
    if ( c == '.' )
    {

      c_temp = cin.get();   //  讀掉點

      if ( isdigit( cin.peek() ) )
      {
        cin.putback( c_temp ); //  放回去
        return true;
      } // if
      else
      {
        cin.putback( c_temp ); //  放回去
        return false;
      } // else



    } // if
    else
      return isdigit( c );
  } // Is_Num()

  bool Is_Num( string str ) //  判斷一字串是否為數字 "3.4" ".5" "5." 都是數字
  {

    stringstream sin( str );
    double d = 0;
    char c = '\0';
    if ( ! ( sin >> d ) )
    {
      //  sin.clear();
      return false;
    } // if

    if ( sin >> c )
    {
      //  sin.clear();
      return false;
    } // if
      //  sin.clear();
    return true;
  } // Is_Num()

  bool Is_OtherToken( char c ) //  判斷是否為  '=' || c == '>' || c == '<' || c == ':' || c == ';'  
  {                            //  注意 這邊沒有'_'是因為'_'只會出現在LETTER"後面" 所以不在這邊判斷
    if ( c == '=' || c == '>' || c == '<' || c == ':' || c == ';' || c == '+' || c == '-' ||
         c == '*' || c == '/' || c == '.' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' ||
         c == '}' || c == '%' || c == '^' || c == '!' || c == '&' || c == '|' || c == '?' || c == ',' ||
         c == '\'' || c == '"' )
      return true; // 尷尬 懶得改了哈哈哈哈哈哈哈哈哈哈哈
    else
      return false;


  } // Is_OtherToken()

  bool Is_Constant( string str )  // 只考慮 0 是否為 " 和 ' 是因為 GetToken 只會取符合的
  {                               // 如果結尾不是 " 和 ' 則在GetToken就會丟出error了
    if ( Is_Num( str ) || str == "true" || str == "false" || str.at( 0 ) == '"' || str.at( 0 ) == '\'' )
      return true;
    else
      return false;


  } // Is_Constant()

  bool Is_Identifier( string str )
  {
    if ( Is_Letter( str.at( 0 ) ) && !Is_Constant( str ) && str != "int" && str != "float" &&
         str != "char" && str != "bool" && str != "string" && str != "void" &&
         str != "if" && str != "else" && str != "while" && str != "do" && str != "return" &&
         str != "cin" && str != "cout" )
      return true;

    else
      return false;

  } // Is_Identifier()

  bool String_Find_the_char( string s, char c ) //  判斷 s 中 是否有 c
  {
    unsigned int idx;

    idx = s.find( c ); //  在s中查找c.
    if ( idx == 4294967295 ) //  不存在。
      return false;
    else //  存在。
      return true;
  } // String_Find_the_char()

  
  
  bool Is_definitioned( string s )
  {
    vector<Information> ::iterator it;
    for ( it = mVariableList->begin() ; it != mVariableList->end() ; it++ )
    {
      if ( it->variable_name == s )
        return true;

    } // for

    return false;


  } // Is_definitioned()


  char Hou_Peek() //  侯氏peek
  {

    if ( gexe || gexe_jump )
    {
      char c;
      c = mCorrect_Token->at( gexe_pos ).at( 0 );

      return c;
    } // if

    char c_temp[256];
    Remove_White_Space();  //  越過whitespace 再偷看 就像偷看人家洗澡時 要先把窗戶打開來一樣:>
    while ( cin.peek() == '/' )  //  註解就像紗窗一樣 如果窗戶打開 還有紗窗??? 讀掉阿 不然呢?
    {
      cin.get();
      if ( cin.peek() == '/' )
      {
        gLine++;

        cin.getline( c_temp, 256 );
      } // if
      else
      {
        cin.putback( '/' );
        return '/';
      } // else

      Remove_White_Space();  //  越過whitespace 再偷看 就像偷看人家洗澡時 要先把窗戶打開來一樣:> 
    } // while

    char ccc = cin.peek();
    return ccc;



  } // Hou_Peek()

  string Float2String( float myFloat )
  {
    stringstream ss;
    ss << myFloat;
    string s( ss.str() );

    return s;
  } // Float2String()

  string WhatStartType( char c ) // 此Token為何種(開頭)
  {
    if ( Is_Letter( c ) ) // 如果是字母(開頭)
      return "Letter";
    else if ( Is_Num( c ) )
      return "Num";
    else if ( Is_OtherToken( c ) )
      return "Other";
    else
      throw LexicalError( c );


  } // WhatStartType()


  string GetToken( bool is_not_peek ) { //  規則: 如果是字母開頭 後面可以是字母、數字、底線
                      //  如果是數字開頭 那後面可以接數字和. 但如果要接.前面要全都是數字才可
                      //  如果是.開頭 後面只能接數字
                      //  其餘的則不能當開頭 如果出現_a 這種則在判斷Type時就會throw例外
                      //  Token 另外有 (c == '=' || c == '>' || c == '<' || c == ':' || c == ';' 
                      //  || c == '_' || c == '+' || c == '-' || c == '*' || c == '/' || c == '.') 已知這幾種

    if ( gexe  || gexe_jump )
    {
      string s;
      s = mCorrect_Token->at( gexe_pos );
      gexe_pos++;
      return s;
    } // if


    string token = ""; // 清空且初始化 雖然外面可能有做過 但還是預防一下:>
    char temp[128]; //  getline用 不太重要
    Remove_White_Space();       // 跳起來
    char c_temp = cin.peek();    //  用來辨別此Token為何種及暫存用
    string type = WhatStartType( c_temp );// 此Token為何種

    while ( Is_Letter( c_temp ) || Is_Num( c_temp ) || Is_OtherToken( c_temp ) || c_temp == '_' )
    { //  字母 數字 上述Token 才有繼續讀的必要 其餘則停下來

      //  不用考慮底線開頭的情況 因為在判斷Type時則會檔掉了 但要考慮字母後面接底線或數字的情況
      if ( type == "Letter" && ( Is_Letter( c_temp ) || c_temp == '_' || Is_Num( c_temp ) ) )
      {
        if ( c_temp == '.' )
          return token;

        c_temp = cin.get();
        token += c_temp;


      } // if
        // 可以是數字 可以是'.' 可以數字開頭也可以是'.'開頭
      else if ( type == "Num" && ( Is_Num( c_temp ) || c_temp == '.' ) )
      {
        if ( ( c_temp == '.' && token != "" && Is_Num_NoDot( token ) ) ||
             ( Is_Num( c_temp ) && c_temp != '.' ) || ( token == "" && Is_Num( c_temp ) ) )
        {  //  出現.  但要確認前面沒出現過. 不然會出現 5.2.3 這種情況
          c_temp = cin.get();
          token += c_temp;
        } // if

        else
          return token; //  token內已經有 3.4 但peek到 . 則不理(文法下一次就會解決它了). 回傳3.4(因為型態不同)


      } // else if
      else if ( type == "Other" && Is_OtherToken( c_temp ) )
      {
        char choose = c_temp;

        if ( choose == '>' ) {      //   '>' '<'  '>='     project2新增 >>
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '=' )   //  '>=' 
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // if
          else if ( cin.peek() == '>' )   //  '>>' 
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // if


          else
          {

            return token;  // '>'
          } // else

        } // if


        else if ( choose == '<' ) {
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '=' )   //   '<='
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // if

          else if ( cin.peek() == '<' )   //  '<<' 
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // if

          else
          {

            return token; // '<'
          } // else

        } // else if



        else if ( choose == '=' )  // project2 新增 '=='
        {
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '=' )    // '==' 的情況
          {
            c_temp = cin.get();
            token += c_temp;

          } // if

          return token;
        } // else if

        else if ( choose == '!' )  // project2 新增 '!'
        {
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '=' )    // '!=' 的情況
          {
            c_temp = cin.get();
            token += c_temp;

          } // if

          return token;
        } // else if

        else if ( choose == '\'' )
        {
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '\n' )
            throw LexicalError( c_temp );

          else
          {
            c_temp = cin.get();
            token += c_temp;
          } // else

          if ( cin.peek() != '\'' )
            throw LexicalError( '\'' );

          else
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // else


        } // else if

        else if ( choose == '"' )
        {
          c_temp = cin.get();
          token += c_temp;
          while ( cin.peek() != '\n' )
          {
            if ( cin.peek() == '\\' && is_not_peek )
            {
              c_temp = cin.get() ;

              if ( cin.peek() == 'n' )
              {
                c_temp = cin.get() ; // 讀掉 \ 和 n
                token += "\n" ;

              } // if

              else
                token += c_temp;



            } // if

            else
            {
              c_temp = cin.get();
              token += c_temp;
            } // else 


            if ( c_temp == '"' )
              return token;


          } // while

          throw LexicalError( '"' );


        } // else if

        else if ( choose == '&' )  // project2 新增 '&'
        {
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '&' )    // '&&' 的情況
          {
            c_temp = cin.get();
            token += c_temp;

          } // if

          return token;
        } // else if

        else if ( choose == '|' )  // project2 新增 '|'
        {
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '|' )    // '||' 的情況
          {
            c_temp = cin.get();
            token += c_temp;

          } // if

          return token;
        } // else if

        else if ( choose == ';' || choose == ',' || choose == '?' || choose == ':' )
        {
          c_temp = cin.get();
          token += c_temp;
          return token;

        } // else if

          //  case '_':  因為'_'不能當開頭 所以不會進到 Other 這個type裡面 所以包在Letter裡面判斷


        else if ( choose == '+' )   //  可以接受 +5 這種數字 所以讀到'+'時 不確定是 運算符號 還是正負符號 所以讀掉'+'後偷看一下後面是不是數字 如果是數字則轉型態
        {
          c_temp = cin.get();
          token += c_temp;        // project2 新增 ++ +=
          if ( cin.peek() == '=' )  // '+='的情況
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // if
          else if ( cin.peek() == '+' )  // '++'的情況
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // else if
          else
          {

          } // else
            // 不做任何事

          return token;

        } // else if




        else if ( choose == '-' ) //  同上
        {
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '=' )  // '-='的情況
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // if
          else if ( cin.peek() == '-' )  // '--'的情況
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // else if
          else
          {

          } // else 
            // 不做任何事

          return token;
        } // else if




        else if ( choose == '*' )
        {

          c_temp = cin.get();
          token += c_temp;

          if ( cin.peek() == '=' )  // '*='的情況
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // if

          return token;
        } // else if


        else if ( choose == '%' )
        {

          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '=' )  // '%='的情況
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // if


          return token;
        } // else if

        else if ( choose == '^' )
        {
          c_temp = cin.get();
          token += c_temp;
          return token;
        } // if


        else if ( choose == '/' ) //  除或是註解
        {
          c_temp = cin.get();
          token += c_temp;
          if ( cin.peek() == '/' )  // 註解的情況 讀掉這一行 重新取type且清空token
          {
            gLine++;
            cin.getline( temp, 128 );     // 讀掉此行後面東西
            token = ""; // 重設token
            Remove_White_Space();

            type = WhatStartType( cin.peek() ); // 重設type

          } // if

          else if ( cin.peek() == '=' )  // '/='的情況
          {
            c_temp = cin.get();
            token += c_temp;
            return token;
          } // else if

          else
            return token;

        } // else if



        else if ( choose == '.' )  //  '.'開頭的數字 例如.5
        {
          c_temp = cin.get();
          token += c_temp;

          if ( !Is_Num( cin.peek() || cin.peek() == '.' ) ) // 因為Is_num會把.5當num  
          {                                                 // 所以如果token是 ..5  這時候第一個點被讀掉 .5會被當合法的

            throw LexicalError( token );
          } // if


          type = "Num";
        } // else if

        else if ( choose == '(' || choose == ')' || choose == '[' || choose == ']' ||
                  choose == '{' || choose == '}' )
        {
          c_temp = cin.get();
          token += c_temp;
          return token;
        } // else if



        else
          throw LexicalError( c_temp );



      } // if


      else //  可能情況例如 a+b(無空白)讀a後下一輪迴圈peek到'+' 但是你的type是Letter 所以中止GetToken 
        return token;

      c_temp = cin.peek(); //  每次結束 要偷看一下下一個字元是什麼垃圾:>

    } // while

    return token;




  } // GetToken()

  string PeekToken() // 如名字 可以偷看下一個token是什麼 應該是沒問題才對...
  {
    if ( gexe  || gexe_jump )
    {
      string s;

      s = mCorrect_Token->at( gexe_pos );

      return s;

    } // if

    string token = GetToken( false );
    for ( int i = token.size() - 1 ; i >= 0 ; i-- )
      cin.putback( token.at( i ) );


    return token;


  } // PeekToken()

  vector<Information> ::iterator  Get_variable( string s )   // 執行階段 遇到id時 需要取值
  {
    vector<Information> ::iterator it;
    for ( it = mVariableList->end() - 1 ; it != mVariableList->begin() ; it-- )
    {
      if ( it->variable_name == s )
        return it;

    } // for


    if ( it->variable_name == s )
      return it;

    return it;


  } // Get_variable()


  // -------------------------------------------文法開始--------------------------------------------

  void User_input()
  {
    int pos = mVariableList->size();
    bool re_defined = false;

    pair<string, vector<string> > aPair;
    Information info;
    map<string, vector<string> > ::iterator map_it;

    string t = PeekToken(); // 存peekToken 
    string str; // 用來存 List...( "aaa" )  的 aaa

    info.boolenop = false;
    info.isFloat = false;
    info.isFunc = false;
    info.value[0] = 0;
    info.data[0] = "";

    gpos = pos;
    ghave_error = false ;


    if ( t == "Done" )
    {
      t = GetToken( true ); // 讀掉Done
      info.token.push_back( t ); // Done放入info中
      int tLine = gLine;


      if ( Hou_Peek() != '(' )
      {
        if ( gLine > tLine )
          g_control = true;

        gpreLine = gLine - tLine;
        gLine = tLine;
        Vector_pop_to_original( pos );

        throw Undefined_identifier( t );



      } // if




      else // 是 '('  之後檢查是否有 ')'
      {
        t = GetToken( true ); // 讀(
        info.token.push_back( t ); // 將 '(' 放入info中


        t = GetToken( true ); // 一定要是 ')'

        if ( t != ")" )
          throw SyntacticError( t );
        else
        {
          info.token.push_back( t ); // 將')' 放入info中
          t = GetToken( true );  // 一 定要是';'

          if ( t != ";" )
            throw SyntacticError( t );
          else
          {
            info.token.push_back( t ); // 將';' 放入info中
            if ( !gexe )
              mCorrect_Token->insert( mCorrect_Token->end(), info.token.begin(), info.token.end() );

            throw End();


          } // else


        } // else


      } // else

    } // if



    else if ( t == "ListVariable" )
    {
      t = GetToken( true ); // 讀 ListVariable
      info.token.push_back( t ); // 將 ListVariable 放入 info中
      t = GetToken( true ); // 讀(
      if ( t != "(" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ( 放入 info中




      t = GetToken( true ); // 讀 constant


      if ( !Is_Constant( t ) || t.at( 0 ) != '"' )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ( 放入 info中
      str.assign( t, 1, t.size() - 2 );  // "xxx" 只取 xxxx

      t = GetToken( true ); // 讀)
      if ( t != ")" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ) 放入 info中

      t = GetToken( true ); // 讀 ;
      if ( t != ";" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ; 放入 info中


      map_it = mInformation_table.find( str );
      if ( map_it != mInformation_table.end() )
      {
        for ( int i = 0 ; i < map_it->second.size() ; i++ )
        {
          cout << map_it->second.at( i );

          if ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) != "[" )
            cout << " ";
          if ( ( i + 1 ) == map_it->second.size() )
            cout << " ";

        } // for

        cout << ";" << endl;

      } // if

      if ( !gexe )
        mCorrect_Token->insert( mCorrect_Token->end(), info.token.begin(), info.token.end() );

      

      if ( gexe && ! ghave_error )
        cout << "Statement executed ..." << endl;

    } // else if

    else if ( t == "ListFunction" )
    {
      t = GetToken( true ); // 讀 ListFunction
      info.token.push_back( t ); // 將 ListFunction 放入 info中
      t = GetToken( true ); // 讀(
      if ( t != "(" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ( 放入 info中




      t = GetToken( true ); // 讀 constant
      if ( !Is_Constant( t ) || t.at( 0 ) != '"' )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 constant  放入 info中
      str.assign( t, 1, t.size() - 2 );  // "xxx" 只取 xxxx
      str = str + "()"; // map中 function名字是存 a() .....
      t = GetToken( true ); // 讀)
      if ( t != ")" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ) 放入 info中

      t = GetToken( true ); // 讀 ;
      if ( t != ";" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ; 放入 info中



      map_it = mInformation_table.find( str );
      if ( map_it != mInformation_table.end() )
      {
        bool start = false;
        bool funcstart = true;
        bool is_while_or_if_no = false; // 不知道該怎麼取名 用看來 是不是 while(5) cout << 4 ;
        bool is_do_while = false;
        for ( int i = 0, k = 0 ; i < map_it->second.size() ; i++ )
        { // k 是用來控制排版的空格數

          if ( !start )
          {
            for ( int c = 0 ; c < k ; c++ )
              cout << " ";

            start = true;
          } // if


          cout << map_it->second.at( i );
          if ( map_it->second.at( i ) != ";" && map_it->second.at( i ) != "{"  &&
               map_it->second.at( i ) != "}" && map_it->second.at( i ) != "++" &&
               map_it->second.at( i ) != "--" &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) != "[" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) != "," ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) != "++" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) != "--" ) )
          {

            if ( ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "(" ) &&
                 map_it->second.at( i ) != "if" && map_it->second.at( i ) != "while" )
            {

            } // if
            else if ( map_it->second.at( i ) == "(" &&
                      ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == ")" ) )
            {
              cout << " ";

            } // else if
            else
            {
              if ( map_it->second.at( i ) == "]" &&
                   ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "," ) )
              {

              } // if
              else
              {
                if ( map_it->second.at( i ) == ")" &&  is_while_or_if_no )
                {
                  if ( ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == ";" ) )
                  {
                    cout << " ";
                    is_do_while = true;
                  } // if

                  else if ( ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) != "{" ) )
                  {
                    cout << endl;
                    k = k + 2;
                    start = false;
                  } // else if



                  else
                  {
                    is_while_or_if_no = false;
                    cout << " ";

                  } // else

                } // if

                else
                {



                  cout << " ";

                } // else


              } // else


            } // else


          } // if

          if ( ( map_it->second.at( i ) == "<<" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "++" ) )
            cout << " ";

          if ( ( map_it->second.at( i ) == "<<" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "--" ) )
            cout << " ";


          if ( ( map_it->second.at( i ) == ">>" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "++" ) )
            cout << " ";

          if ( ( map_it->second.at( i ) == ">>" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "--" ) )
            cout << " ";




          if ( ( map_it->second.at( i ) == "++" ) &&
               ( ( i + 1 ) != map_it->second.size() && !Is_Identifier( map_it->second.at( i + 1 ) ) ) )
            cout << " ";

          if ( ( map_it->second.at( i ) == "--" ) &&
               ( ( i + 1 ) != map_it->second.size() && !Is_Identifier( map_it->second.at( i + 1 ) ) ) )
            cout << " ";


          if ( ( map_it->second.at( i ) == "+" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "++" ) )
            cout << " ";

          if ( ( map_it->second.at( i ) == "-" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "++" ) )
            cout << " ";

          if ( ( map_it->second.at( i ) == "+" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "--" ) )
            cout << " ";

          if ( ( map_it->second.at( i ) == "-" ) &&
               ( ( i + 1 ) != map_it->second.size() && map_it->second.at( i + 1 ) == "--" ) )
            cout << " ";



          if ( map_it->second.at( i ) == "while" || map_it->second.at( i ) == "if" )
            is_while_or_if_no = true;



          if ( map_it->second.at( i ) == "(" )
            funcstart = false;

          if ( map_it->second.at( i ) == ";" )
          {
            cout << endl; // 遇到分號 換行

            if ( is_while_or_if_no && !is_do_while )
              k = k - 2;

            is_do_while = false;

            is_while_or_if_no = false;

            start = false;
          } // if


          if ( map_it->second.at( i ) == "{" )
          {
            cout << endl; // 遇到{ 換行
            start = false;
            k = k + 2;
          } // if


          if ( ( i + 1 ) < map_it->second.size() && map_it->second.at( i + 1 ) == "}" )
          {

            k = k - 2;
          } // if

          if ( map_it->second.at( i ) == "}" )
          {
            cout << endl; // 遇到} 換行
            start = false;
          } // if




        } // for


      } // if

      if ( !gexe )
        mCorrect_Token->insert( mCorrect_Token->end(), info.token.begin(), info.token.end() );

      

      if ( gexe && ! ghave_error )
        cout << "Statement executed ..." << endl;


    } // else if

    else if ( t == "ListAllVariables" )
    {
      t = GetToken( true ); // 讀 ListAllVariables
      info.token.push_back( t ); // 將 ListAllVariables 放入 info中
      t = GetToken( true ); // 讀 (
      if ( t != "(" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ( 放入 info中
      t = GetToken( true );  // 讀 )

      if ( t != ")" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ) 放入 info中

      t = GetToken( true ); // 讀 ;

      if ( t != ";" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ; 放入 info中
      map_it = mInformation_table.begin();

      while ( map_it != mInformation_table.end() )
      {
        if ( map_it->first.at( map_it->first.size() - 1 ) != ')' )
          cout << map_it->first << endl;

        map_it++;

      } // while

      if ( !gexe )
        mCorrect_Token->insert( mCorrect_Token->end(), info.token.begin(), info.token.end() );

      

      if ( gexe && ! ghave_error )
        cout << "Statement executed ..." << endl;

    } // else if

    else if ( t == "ListAllFunctions" )
    {
      t = GetToken( true ); // 讀 ListAllVariables
      info.token.push_back( t ); // 將 ListAllVariables 放入 info中
      t = GetToken( true ); // 讀 (
      if ( t != "(" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ( 放入 info中
      t = GetToken( true );  // 讀 )

      if ( t != ")" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ) 放入 info中

      t = GetToken( true ); // 讀 ;

      if ( t != ";" )
      {
        Vector_pop_to_original( pos );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 將 ; 放入 info中

      map_it = mInformation_table.begin();

      while ( map_it != mInformation_table.end() )
      {
        if ( map_it->first.at( map_it->first.size() - 1 ) == ')' )
          cout << map_it->first << endl;

        map_it++;

      } // while

      if ( !gexe )
        mCorrect_Token->insert( mCorrect_Token->end(), info.token.begin(), info.token.end() );

      

      if ( gexe && ! ghave_error )
        cout << "Statement executed ..." << endl;

    } // else if



    else if ( t == "void" || t == "int" || t == "char" || t == "float" || t == "string" ||
              t == "bool" ) // definition路線
    {


      Definition( info, pos );

      if ( info.isFunc )
      {

        map< string, vector<string> > ::iterator it;
        it = mInformation_table.find( info.name.at( 0 ) + "()" );

        if ( it != mInformation_table.end() ) // 代表已經有相同的變數名稱 所以要覆蓋掉
        {
          re_defined = true;
          mInformation_table.erase( it );
        } // if


        aPair.first = info.name.at( 0 ) + "()";
        aPair.second = info.token;
        mInformation_table.insert( aPair );
        if ( re_defined )
        {

          if ( gexe && ! ghave_error )
            cout << "New definition of " + info.name.at( 0 ) + "()" + " entered ..." << endl;

        } // if

        else
        {
          if ( gexe && ! ghave_error )
            cout << "Definition of " + info.name.at( 0 ) + "()" + " entered ..." << endl;

        } // else


      } // if
      else // int a , b ,c  || int a[10] ,b ,c [5 ] ....
      {

        int size = info.name.size(); // int a, b , c -> size = 3
        int k = 1;
        map< string, vector<string> > ::iterator it;

        for ( int i = 0 ; i < size ; i++ )
        {
          vector<string> tv; // 暫存用的而已
          tv.push_back( info.token.at( 0 ) ); // type   int char...

          aPair.first = info.name.at( i );
          bool ttt = false; // 控制下方 for迴圈
          for ( ; k < info.token.size() && !ttt ; k++ )
          {
            if ( info.token.at( k ) == "," || info.token.at( k ) == ";" )
            {

              ttt = true;
            } // if

            if ( !ttt )
              tv.push_back( info.token.at( k ) );

          } // for

          it = mInformation_table.find( info.name.at( i ) );
          if ( it != mInformation_table.end() ) // 代表已經有相同的變數名稱 所以要覆蓋掉
          {
            re_defined = true;
            mInformation_table.erase( it );
          } // if


          aPair.second = tv;
          mInformation_table.insert( aPair );



          if ( re_defined )
          {

            if ( gexe && ! ghave_error )
              cout << "New definition of " + info.name.at( i ) + " entered ..." << endl;

          } // if
          else
          {
            if ( gexe && ! ghave_error )
              cout << "Definition of " + info.name.at( i ) + " entered ..." << endl;

          } // else

          tv.clear();
          re_defined = false;

        } // for


      } // else

      if ( !gexe )
        mCorrect_Token->insert( mCorrect_Token->end(), info.token.begin(), info.token.end() );

    } // else if

    else // statement 路線
    {

      Statement( info, pos );

      if ( !gexe )
        mCorrect_Token->insert( mCorrect_Token->end(), info.token.begin(), info.token.end() );

      if ( gexe && ! ghave_error )
        cout << "Statement executed ..." << endl;

    } // else

  } // User_input()

  void Definition( Information &info, int p )
  {
    string t = PeekToken();
    string id;

    Information temp;
    temp.boolenop = false;
    temp.isFloat = false;
    temp.isFunc = false;
    temp.variable_type = "";
    temp.value[0] = 0;
    temp.data[0] = "";




    if ( t == "void" ) // VOID Identifier function_definition_without_ID
    {

      t = GetToken( true );  // 讀掉void
      info.isFunc = true; // void 開頭 是 function
      info.token.push_back( t ); // 將void 放入 info中
      t = GetToken( true ); //  Identifier
      if ( !Is_Identifier( t ) )
        throw SyntacticError( t );
      else  // 是ID 將ID存起來
      {
        id = t + "()";

        info.token.push_back( t ); // 將 id 放入info 中
        info.name.push_back( t ); //  將 id 放入name 中

        Function_definition_without_ID( info, p );
      } // else 



    } // if
    else // type_specifier Identifier function_definition_or_declarators
    {
      Type_specifier( info, p );

      t = GetToken( true ); // Identifier

      if ( !Is_Identifier( t ) )
        throw SyntacticError( t );

      else  // 是ID 將ID存起來
      {
        id = t;
        if ( Hou_Peek() == '(' )
          id = id + "()";




        info.token.push_back( t ); // 將 id 放入info 中
        info.name.push_back( t ); //  將 id 放入name 中
        Function_definition_or_declarators( info, p );
      } // else 

    } // else

    temp.variable_name = id;
    temp.variable_type = info.variable_type;
    mVariableList->push_back( temp );

  } // Definition()

  void Type_specifier( Information &info, int p )
  {
    string t = GetToken( true );
    if ( t != "int" && t != "char" && t != "float" && t != "string" && t != "bool" )
    {

      Vector_pop_to_original( p );
      throw SyntacticError( t );
    } // if

    else
    {
      info.token.push_back( t );
      info.variable_type = t;

    } // else


  } // Type_specifier()

  void Function_definition_or_declarators( Information &info, int p )
  { // function_definition_or_declarators -> function_definition_without_ID  | rest_of_declarators

    char t = Hou_Peek();
    if ( t == '(' )
    {
      info.isFunc = true;
      Function_definition_without_ID( info, p );
    } // if

    else
      Rest_of_declarators( info, p );

  } // Function_definition_or_declarators()

  void Rest_of_declarators( Information &info, int p )
  { // rest_of_declarators : [  '[' Constant ']'  ] { ',' Identifier [ '[' Constant ']' ] } ';'

    vector<Information> *t_variable = new vector<Information>();
    Information temp;
    temp.boolenop = false;
    temp.isFloat = false;
    temp.isFunc = false;
    temp.value[0] = 0;
    temp.data[0] = "";

    string t;
    char c = Hou_Peek();
    if ( c == '[' )
    {
      t = GetToken( true ); // 把 [ 讀掉
      info.token.push_back( t ); // 把 [ 放到 info 裡面
      t = GetToken( true ); // 讀 constant
      if ( !Is_Constant( t ) )
      {
        Vector_pop_to_original( p ); // 把剛剛function裡的變數從VariableList中pop掉
        throw SyntacticError( t );
      } // if

      else
      {
        info.token.push_back( t ); // 把 constant 放到 info 裡面
        t = GetToken( true ); // 讀 ]
        if ( t != "]" )
        {
          Vector_pop_to_original( p ); // 把剛剛function裡的變數從VariableList中pop掉
          throw SyntacticError( t );
        } // if
        else
          info.token.push_back( t ); // 把 ] 放到 info 裡面

      } // else

    } // if

    while ( Hou_Peek() == ',' )
    {
      t = GetToken( true ); // 把 , 讀掉
      info.token.push_back( t ); // 把 , 放入 info裡
      t = GetToken( true ); // 讀 ID

      if ( !Is_Identifier( t ) )
      {
        Vector_pop_to_original( p ); // 把剛剛function裡的變數從VariableList中pop掉
        throw SyntacticError( t );
      } // if
      else
      {
        temp.variable_name = t;
        temp.variable_type = info.variable_type;
        t_variable->push_back( temp );
        info.token.push_back( t ); // 把 id 放入 info裡
        info.name.push_back( t ); // 把 id 放入 name 裡

      } // else

      c = Hou_Peek();
      if ( c == '[' )
      {
        t = GetToken( true ); // 把 [ 讀掉
        info.token.push_back( t ); // 把 [ 放到 info 裡面
        t = GetToken( true ); // 讀 constant
        if ( !Is_Constant( t ) )
        {
          Vector_pop_to_original( p ); // 把剛剛function裡的變數從VariableList中pop掉
          throw SyntacticError( t );
        } // if
        else
        {
          info.token.push_back( t ); // 把 constant 放到 info 裡面
          t = GetToken( true ); // 讀 ]
          if ( t != "]" )
          {
            Vector_pop_to_original( p ); // 把剛剛function裡的變數從VariableList中pop掉
            throw SyntacticError( t );
          } // if
          else
            info.token.push_back( t ); // 把 ] 放到 info 裡面

        } // else

      } // if


    } // while

    t = GetToken( true ); // 讀分號
    if ( t != ";" )
    {
      Vector_pop_to_original( p ); // 把剛剛function裡的變數從VariableList中pop掉
      throw SyntacticError( t );
    } // if

    else
      info.token.push_back( t );  // 把 ; 放到info裡


    mVariableList->insert( mVariableList->end(), t_variable->begin(), t_variable->end() );


    delete t_variable;

  } // Rest_of_declarators()

  void Function_definition_without_ID( Information &info, int p )
  {  // function_definition_without_ID : '(' [ VOID | formal_parameter_list ] ')' compound_statement
    int t_pos = mVariableList->size(); // pop時才知道要pop到什麼地方
    string t = GetToken( true ); // 讀掉 (
    char c;

    if ( t != "(" )
    {
      Vector_pop_to_original( p );
      throw SyntacticError( t );
    } // if

    else
    {
      info.token.push_back( t ); // 將 ( 放入info 裡
      c = Hou_Peek(); // 偷看一下 因為是 [ ]
      if ( c != ')' )  // 如果不是 )  就有可能是 void 或是 formal_par.....
      {
        t = PeekToken();

        if ( t == "void" )
        {
          t = GetToken( true ); // 讀掉 void
          info.token.push_back( t ); // 將 void 放入info裡
        } // if
        else
          Formal_parameter_list( info, p );

      } // if

      t = GetToken( true );  // 讀掉 )
      if ( t != ")" )
      {
        Vector_pop_to_original( p ); // 丟error前 pop掉之前的變數
        throw SyntacticError( t );
      } // if


      else
        info.token.push_back( t ); // 將 ) 放入 info裡

      Compound_statement( info, p );
      Vector_pop_to_original( t_pos );

    } // else

  } // Function_definition_without_ID()

  void Formal_parameter_list( Information &info, int p )
  { // formal_parameter_list :  type_specifier [ '&' ]  Identifier [  '[' Constant ']'  ] 
    // {  ','  type_specifier  [ '&' ]  Identifier [  '[' Constant ']'  ] }

    Information temp;
    temp.boolenop = false;
    temp.isFloat = false;
    temp.isFunc = false;
    temp.value[0] = 0;
    temp.data[0] = "";


    string t;
    char c;
    vector<Information> *t_variable = new vector<Information>();

    Type_specifier( info, p );
    t = PeekToken();

    if ( t == "&" )
    {
      t = GetToken( true ); // 讀掉 &
      info.token.push_back( t ); // 將 & 放入 info裡
    } // if

    t = GetToken( true ); // 讀 id
    if ( !Is_Identifier( t ) )
    {
      Vector_pop_to_original( p );
      throw SyntacticError( t );
    } // if

    else
    {
      temp.variable_name = t;
      t_variable->push_back( temp );
      info.token.push_back( t ); // 將 id 放入 info裡

    } // else


    c = Hou_Peek();
    if ( c == '[' )
    {
      t = GetToken( true ); // 把 [ 讀掉
      info.token.push_back( t ); // 把 [ 放到 info 裡面
      t = GetToken( true ); // 讀 constant
      if ( !Is_Constant( t ) )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if
      else
      {
        info.token.push_back( t ); // 把 constant 放到 info 裡面
        t = GetToken( true ); // 讀 ]
        if ( t != "]" )
        {

          Vector_pop_to_original( p );
          throw SyntacticError( t );
        } // if

        else
          info.token.push_back( t ); // 把 ] 放到 info 裡面

      } // else

    } // if

    while ( Hou_Peek() == ',' )
    {
      temp.boolenop = false;
      temp.isFloat = false;
      temp.isFunc = false;
      temp.value[0] = 0;
      temp.data[0] = "";


      t = GetToken( true ); // 讀 ,
      info.token.push_back( t ); // 把 , 放入 info


      Type_specifier( info, p );

      t = PeekToken();

      if ( t == "&" )
      {
        t = GetToken( true ); // 讀掉 &
        info.token.push_back( t ); // 將 & 放入 info裡
      } // if

      t = GetToken( true ); // 讀 id
      if ( !Is_Identifier( t ) )
      {

        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      else
      {
        temp.variable_name = t;
        t_variable->push_back( temp );
        info.token.push_back( t ); // 將 id 放入 info裡
      } // else

      c = Hou_Peek();

      if ( c == '[' )
      {
        t = GetToken( true ); // 把 [ 讀掉
        info.token.push_back( t ); // 把 [ 放到 info 裡面
        t = GetToken( true ); // 讀 constant
        if ( !Is_Constant( t ) )
        {
          Vector_pop_to_original( p );
          throw SyntacticError( t );
        } // if

        else
        {
          info.token.push_back( t ); // 把 constant 放到 info 裡面
          t = GetToken( true ); // 讀 ]
          if ( t != "]" )
          {

            Vector_pop_to_original( p );
            throw SyntacticError( t );
          } // if

          else
            info.token.push_back( t ); // 把 ] 放到 info 裡面

        } // else

      } // if




    } // while

    mVariableList->insert( mVariableList->end(), t_variable->begin(), t_variable->end() );
    delete t_variable;
  } // Formal_parameter_list()

  void Compound_statement( Information &info, int p )
  { // compound_statement : '{' { declaration | statement } '}'
    int t_pos = mVariableList->size(); // pop時才知道要pop到什麼地方  是紀錄此{} 層的 而不是 跟p 不同




    string t = GetToken( true ); // 讀 {

    if ( t != "{" )
    {
      Vector_pop_to_original( p ); // 丟error前 pop掉之前的變數
      throw SyntacticError( t );
    } // if

    else // 是 {
      info.token.push_back( t ); // 把 { 放入 info 裡 


    while ( Hou_Peek() != '}' )  // 不是}就是呼叫 declaration | statement 至於error? 交給他們處理就好
    {
      t = PeekToken();
      if ( t == "int" || t == "char" || t == "string" ||
           t == "bool" || t == "float" ) Declaration( info, p );

      else
        Statement( info, p );


    } // while

    t = GetToken( true ); // 讀 }
    if ( t != "}" )
    {
      Vector_pop_to_original( p ); // 丟error前 pop掉之前的變數
      throw SyntacticError( t );
    } // if

    else
    {
      Vector_pop_to_original( t_pos ); // 把剛剛function裡的變數從VariableList中pop掉
      info.token.push_back( t ); // 把 } 放入 info 裡
    } // else




  } // Compound_statement()

  void Declaration( Information &info, int p ) // declaration : type_specifier Identifier rest_of_declarators
  {
    string t;
    Information temp;
    temp.data[0] = "";
    temp.value[0] = 0;
    temp.isFloat = false;


    Type_specifier( info, p );

    t = GetToken( true ); //  讀 Identifier

    if ( !Is_Identifier( t ) )
    {
      Vector_pop_to_original( p ); // 丟error前 pop掉之前的變數
      throw SyntacticError( t );
    } // if
    else
    {
      temp.variable_name = t;
      mVariableList->push_back( temp ); // 把 id 放到 variable 中
      info.token.push_back( t ); // 把 id 放到 info 裡
    } // else 

    Rest_of_declarators( info, p );


  } // Declaration()

  void Statement( Information &info, int p )
  {
    string t = PeekToken();
    bool conditional = false ;


    if ( t == ";" )
    {
      t = GetToken( true ); // 把";"讀掉
      info.token.push_back( t ); // 把 ";" 放到 info 裡


    } // if

    else if ( t == "return" )
    {
      t = GetToken( true );
      info.token.push_back( t ); // 把 "return" 放到 info 裡

      if ( Hou_Peek() != ';' )
        Expression( info, p );

      t = GetToken( true ); // 把 ";"讀掉

      if ( t != ";" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if


      info.token.push_back( t ); // 把 ";" 放到 info 裡 


    } // else if

    else if ( t == "{" )
      Compound_statement( info, p );


    else if ( t == "if" )
    {
      t = GetToken( true ); // 讀掉if
      info.token.push_back( t ); // 把 if 放入info
      t = GetToken( true ); // 讀 (
      if ( t != "(" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 把 "(" 放到 info 裡 

      Expression( info, p );

      conditional = info.value[0] ;

      info.boolenop = false;
      info.variable_type = "int";
      info.sign = "+" ;


      t = GetToken( true ); // 讀 )

      if ( t != ")" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 把 ")" 放到 info 裡 

      if ( !gexe )     
        Statement( info, p );
      else  // gexe
      {

        if ( conditional == true )   // 條件成立 做運算
          Statement( info, p );
        else  // 條件不成立 跳跳跳~
        {
          gexe = false ;
          gexe_jump = true ;

          Statement( info, p );

          gexe = true ;
          gexe_jump = false  ;


        } // else


      } // else



      if ( Hou_Peek() == 'e' )
      {
        t = PeekToken(); // 看是不是 else
        if ( t == "else" )
        {
          t = GetToken( true ); // 讀 else
          info.token.push_back( t ); // 把 else 放到 info 裡 

          if ( !gexe ) 
            Statement( info, p );
          else // 運算階段
          {

            if ( conditional == false ) // if的條件是 false 所以執行else
            {
              Statement( info, p );
            } // if
            else // 跳跳跳
            {
              gexe = false ;
              gexe_jump = true ;

              Statement( info, p );

              gexe = true ;
              gexe_jump = false  ;

            } // else 

          } // else



        } // if

      } // if

    } // else if


    else if ( t == "while" )
    { 
      int temp_pos ; // 紀錄 while的起始位置

      

      t = GetToken( true ); // 讀 while
      info.token.push_back( t ); // 把 "while" 放到 info 裡 
      t = GetToken( true ); // 讀 (
      if ( t != "(" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 把 "(" 放到 info 裡 
      
      if ( gexe )
        temp_pos = gexe_pos ;

      Expression( info, p );


      conditional = info.value[0] ;

      t = GetToken( true ); // 讀 )

      if ( t != ")" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 把 ")" 放到 info 裡 
      bool run = true ;

      if ( !gexe )
        Statement( info, p );
      else // 運算階段
      {
        do
        {


          info.boolenop = false;
          info.variable_type = "int";
          info.sign = "+" ;
          


          if ( conditional == true )
            Statement( info, p );
          else
          {

       
            gexe = false ;
            gexe_jump = true ;

            Statement( info, p );

            gexe = true ;
            gexe_jump = false ;

            run = false ;

          } // else

          if ( run )
          {
            info.boolenop = false;
            info.variable_type = "int";
            info.sign = "+" ;
            info.value[0] = 0 ;
            info.isFloat = false ;
            

            gexe_pos = temp_pos ;
            Expression( info, p );

            gexe_pos++ ; // )
            
            conditional = info.value[0] ;

          } // if



        }
        while ( run ) ;



      } // else

    } // else if


    else if ( t == "do" )
    {
      t = GetToken( true ); // 讀 do
      info.token.push_back( t ); // 把 "do" 放到 info 裡 
      Statement( info, p );
      t = GetToken( true ); // 讀 while

      if ( t != "while" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );

      } // if

      info.token.push_back( t ); // 把 "while" 放到 info 裡 

      t = GetToken( true ); // 讀 (
      if ( t != "(" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 把 "(" 放到 info 裡 

      Expression( info, p );


      t = GetToken( true ); // 讀 )

      if ( t != ")" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); // 把 ")" 放到 info 裡 

      t = GetToken( true ); // 把 ";"讀掉

      if ( t != ";" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if


      info.token.push_back( t ); // 把 ";" 放到 info 裡 




    } // else if

    else
    {


      Expression( info, p );

      t = GetToken( true ); // 把 ";"讀掉

      if ( t != ";" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if


      info.token.push_back( t ); // 把 ";" 放到 info 裡 

    } // else



  } // Statement()

  void Expression( Information &info, int p )
  {
    string t;
    Basic_expression( info, p );

    while ( Hou_Peek() == ',' )
    {
      t = GetToken( true ); // 讀 ,
      info.token.push_back( t ); // 把 "," 放到 info 裡 
      Basic_expression( info, p );

    } // while

  } // Expression()

  void Basic_expression( Information &info, int p )
  {



    string t = PeekToken();
    int tLine;
    if ( Is_Identifier( t ) || t == "cout" || t == "cin" ) // Identifier rest_of_Identifier_started_basic_exp
    {
      t = GetToken( true ); // 讀 id    此處要判斷此id是否被宣告過
      tLine = gLine;
      if ( Hou_Peek() == '(' )  // 代表此id是function
      {


        if ( !Is_definitioned( t + "()" ) && t != "cout " && t != "cin" )
        {
          if ( gLine > tLine )
            g_control = true;

          gpreLine = gLine - tLine;
          gLine = tLine;
          Vector_pop_to_original( p );
          throw Undefined_identifier( t );
        } // if

      } // if
      else
      {

        if ( !Is_definitioned( t ) && t != "cout" && t != "cin" )
        {

          if ( gLine > tLine )
            g_control = true;

          gpreLine = gLine - tLine;
          gLine = tLine;
          Vector_pop_to_original( p );
          throw Undefined_identifier( t );
        } // if

      } // else



      info.token.push_back( t ); // 把 id 放到 info 裡 


      if ( t == "cout" )
      {
        gis_cout = true ;

        while ( PeekToken() == "<<" )
        {
          
          t = GetToken( true ); // 讀 <<
          info.token.push_back( t ); // 把 << 放到 info 裡 
          Maybe_additive_exp( info, p );   // 幹!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

         

          while ( PeekToken() == "<" || PeekToken() == ">" || PeekToken() == "<=" || PeekToken() == ">=" )
          {
            int value ;
            value = info.value[0];
            t = GetToken( true ); // 讀  < 或 > 或 <= 或 >=
            info.token.push_back( t ); //   把 < 或 > 或 <= 或 >= 放到 info 裡 
            Maybe_shift_exp( info, p );

            if ( gexe )
            {
              if ( t == "<" )
              {
                if ( value < info.value[0] )
                  info.value[0] = 1;
                else
                  info.value[0] = 0;


              } // if

              else if ( t == ">" )
              {
                if ( value > info.value[0] )
                  info.value[0] = 1;
                else
                  info.value[0] = 0;

              } // else if

              else if ( t == "<=" )
              {
                if ( value <= info.value[0] )
                  info.value[0] = 1;
                else
                  info.value[0] = 0;

              } // else if

              else // >=
              {

                if ( value >= info.value[0] )
                  info.value[0] = 1;
                else
                  info.value[0] = 0;

              } // else

              info.boolenop = true;



            } // if


          } // while


          while ( PeekToken() == "==" || PeekToken() == "!=" )
          {
            int value ;
            string t_data ;

            if ( gexe )
            {
              if ( info.variable_type != "string" &&  info.variable_type != "char" )
                value = info.value[0];
              else
                t_data = info.data[0];

            } // if


            t = GetToken( true ); // 讀  == 或 !=
            info.token.push_back( t ); //   把 == 或 != 放到 info 裡 
            Maybe_relational_exp( info, p );


            if ( gexe )
            {
              if ( t == "==" )
              {
                if ( info.variable_type != "string" &&  info.variable_type != "char" )
                {
                  if ( info.value[0] == value )
                    info.value[0] = 1;
                  else
                    info.value[0] = 0;
                } // if
                else
                {
                  if ( info.data[0] == t_data )
                    info.value[0] = 1;
                  else
                    info.value[0] = 0;

                } // else



              } // if
              else
              {

                if ( info.variable_type != "string" &&  info.variable_type != "char" )
                {
                  if ( info.value[0] != value )
                    info.value[0] = 1;
                  else
                    info.value[0] = 0;
                } // if
                else
                {
                  if ( info.data[0] != t_data )
                    info.value[0] = 1;
                  else
                    info.value[0] = 0;

                } // else


              } // else


            } // if


            info.boolenop = true;

          } // while


          while ( PeekToken() == "&&" )
          {
            int value ;
            value = info.value[0];
            t = GetToken( true ); // 讀 &&
            info.token.push_back( t ); //   把 && 放到 info 裡 
            Maybe_bit_OR_exp( info, p );

            if ( gexe )
            {
              if ( info.value[0] >= 1 && value >= 1 )
                info.value[0] = 1;
              else
                info.value[0] = 0;

            } // if

            info.boolenop = true;

          } // while

          while ( PeekToken() == "||" )
          {
            int value ;
            value = info.value[0];
            t = GetToken( true ); // 讀 ||
            info.token.push_back( t ); //   把 || 放到 info 裡 
            Maybe_logical_AND_exp( info, p );

            if ( gexe )
            {
              if ( info.value[0] >= 1 || value >= 1 )
                info.value[0] = 1;
              else
                info.value[0] = 0;

            } // if


            info.boolenop = true;



          } // while

          if ( Hou_Peek() == '?' )
          {
            bool conditional = false ;

            t = GetToken( true ); // 讀 ?
            info.token.push_back( t ); //   把 ? 放到 info 裡 

            if ( !gexe )
              Basic_expression( info, p );  // 非運算階段的

            else // 運算階段
            {

              if ( conditional == true )
              {
                info.boolenop = false;
                info.variable_type = "int";
                info.sign = "+" ;


                Basic_expression( info, p );


              } // if
              else  // 跳過此文法不做
              {
                gexe = false ;
                gexe_jump = true ;

                Basic_expression( info, p );

                gexe = true ;
                gexe_jump = false ;

              } // else

            } // else




            t = GetToken( true ); // 讀 :



            if ( t != ":" )
            {
              Vector_pop_to_original( p );
              throw SyntacticError( t );

            } // if

            info.token.push_back( t ); //   把 : 放到 info 裡 


            if ( !gexe )
              Basic_expression( info, p );  // 非運算階段的

            else // 運算階段
            {

              if ( conditional == false )
              {
                info.boolenop = false;
                info.variable_type = "int";
                info.sign = "+" ;
                Basic_expression( info, p );

              } // if
              else  // 跳過此文法不做
              {
                gexe = false ;
                gexe_jump = true ;

                Basic_expression( info, p );

                gexe = true ;
                gexe_jump = false ;


              } // else

            } // else


          } // if



          while ( Hou_Peek() == ',' )
          {

            info.boolenop = false;
            info.variable_type = "int";
            info.sign = "+" ;

            t = GetToken( true ); // 讀 ,
            info.token.push_back( t ); // 把 "," 放到 info 裡 
            

            Maybe_additive_exp( info, p );


          } // while




          if ( gexe )
          {
            if ( !info.boolenop )
            {

              if ( info.variable_type == "string" || info.variable_type == "char" )
              {
                if ( info.data[0] == "\n" )
                {
                  cout << endl;

                } // if
                else
                {

                  cout << info.data[0];
                 
                  if ( info.data[0].at( 0 ) == 'L' && info.data[0].at( 1 ) == 'i'
                       && info.data[0].at( 2 ) == 'n' && info.data[0].at( 3 ) == 'e' )
                    ghave_error = true ;

                } // else


              } // if

              else
                printf( "%.f", info.value[0] );

            } // if
            else
            {

              if ( info.value[0] == 1  )
                cout << "true";
              else
                cout << "false";

            } // else

            info.boolenop = false;
            info.variable_type = "int";
            info.sign = "+" ;
            


          } // if






        } // while

        gis_cout = false ;


      } // if

      else if ( t == "cin" )
      {

        while ( PeekToken() == ">>" )
        {
          t = GetToken( true ); // 讀 >>
          info.token.push_back( t ); // 把 >> 放到 info 裡 
          Maybe_additive_exp( info, p );




        } // while



      } // else if

      else
      {
        int value = 0;
        vector<Information> ::iterator it;
        if ( gexe )
        {
          it = Get_variable( t );
          info.value[0] = it->value[0];
          info.variable_name = t;

        } // if 


        Rest_of_Identifier_started_basic_exp( info, p );

      } // else




    } // if
    else if ( t == "++" || t == "--" ) // ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
    {
      string op = ""; // 紀錄運算符號
      int value = 0;
      int p = 0; // 是否有陣列? 如果不是陣列則更改0的問題
      vector<Information> ::iterator it;

      t = GetToken( true ); // 讀 ++ 或 --
      op = t;
      info.token.push_back( t ); // 把 ++ or -- 放到 info 裡 
      t = GetToken( true ); // 讀 id    此處要判斷此id是否被宣告過

      if ( gexe )
      {

        it = Get_variable( t );

        if ( op == "++" )     // 前運算 直接去id table 找到那個變數++  or  --
        {

          info.pp_mm = "++";

        } // if
        else // --
        {

          info.pp_mm = "--";



        } // else


        info.variable_name = t;


      } // if




      if ( !Is_Identifier( t ) )   // 先檢查是不是ID
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );

      } // if


      if ( !Is_definitioned( t ) ) // 再檢查此id有沒有宣告過
      {
        Vector_pop_to_original( p );
        throw Undefined_identifier( t );
      } // if



      info.token.push_back( t ); // 確認無誤  把 id 放到 info 裡

      Rest_of_PPMM_Identifier_started_basic_exp( info, p );




    } // else if

    else if ( t == "+" || t == "-" || t == "!" ) // sign{ sign } signed_unary_exp romce_and_romloe
    {
      
      string sign_temp ;
      Sign( info, p );

      sign_temp = info.sign ;

      while ( PeekToken() == "+" || PeekToken() == "-" || PeekToken() == "!" )
      {
        Sign( info, p );

        if ( sign_temp == info.sign )
          info.sign = "+" ;

      } // while

      Signed_unary_exp( info, p );
      Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp( info, p );


    } // else if

    else if ( Is_Constant( t ) || t == "(" )
    { // ( Constant | '(' expression ')' ) rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp
      t = GetToken( true ); // 讀 constant 或是 (

      if ( t == "(" )
      {
        info.token.push_back( t ); //   把 ( 放到 info 裡
        Expression( info, p );

        t = GetToken( true ); // 讀 )
        if ( t != ")" )
        {
          Vector_pop_to_original( p );
          throw SyntacticError( t );
        } // if

        info.token.push_back( t ); //   把 ) 放到 info 裡

      } // if
      else if ( Is_Constant( t ) ) // Constant
      {
        info.token.push_back( t ); //   把 constant 放到 info 裡 

        if ( gexe )
        {

          if ( Is_Num( t ) )
          {

            if ( String_Find_the_char( t, '.' ) )
              info.isFloat = true;

            if ( info.isFloat )
              info.value[0] = atof( t.c_str() ) ;

            else
              info.value[0] = atoi( t.c_str() );


           


          } // if
          else if ( t == "true" || t == "false" )
          {
            if ( t == "true" )
              info.value[0] = 1;
            else
              info.value[0] = 0;

            info.boolenop = true ;


          } // else if

          else
          {
            if ( t != "\"\\n\"" )
              info.data[0] = t.assign( t, 1, t.size() - 2 );
            else
              info.data[0] = "\n";

          } // else





        } // if





      } // if


      else
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );

      } // else


      Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp( info, p );





    } // else if

    else
    {
      Vector_pop_to_original( p );
      throw SyntacticError( t );

    } // else




  } // Basic_expression()

  void Rest_of_Identifier_started_basic_exp( Information &info, int p )
  { // rest_of_Identifier_started_basic_exp : [ '[' expression ']' ] 
    // ( assignment_operator   basic_expression  | [PP | MM] romce_and_romloe ) 
    // | '('[actual_parameter_list] ')' romce_and_romloe
    string t, id, t_data;

    vector<Information> ::iterator it;
    int array_pos = -1;
    int value; // 紀錄原本的值



    if ( gexe )
      id = mCorrect_Token->at( gexe_pos - 1 );

    char c = Hou_Peek();


    if ( c == '(' ) // '('[actual_parameter_list] ')' romce_and_romloe
    {
      t = GetToken( true ); // 讀 (
      info.token.push_back( t ); //   把 ( 放到 info 裡 
      c = Hou_Peek(); // 如果不是) 就呼叫 actual_parameter_list

      if ( c != ')' )
        Actual_parameter_list( info, p );

      t = GetToken( true ); // 讀 )
      if ( t != ")" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); //   把 ) 放到 info 裡 

      Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp( info, p );



    } // if


    else // ( assignment_operator basic_expression   | [ PP | MM ] romce_and_romloe  ) 
    {




      if ( gexe )
      {
        it = Get_variable( id );

        if ( it->variable_type != "string" &&  it->variable_type != "char" &&  it->variable_type != "char" )
          info.value[0] = it->value[0];
        else
          info.data[0] = it->data[0];

        info.variable_type = it->variable_type;

      } // if



      if ( c == '[' )
      {
        t = GetToken( true ); // 讀 [
        info.token.push_back( t ); //   把 [ 放到 info 裡 
        Expression( info, p );

        if ( gexe )
        {
          array_pos = info.value[0] ;

          if ( it->variable_type != "string" &&  it->variable_type != "char" )
            info.value[0] = it->value[array_pos];
          else
            info.data[0] = it->data[array_pos];
        
        } // if


        t = GetToken( true ); // 讀 ]
        if ( t != "]" )
        {
          Vector_pop_to_original( p );
          throw SyntacticError( t );
        } // if

        info.token.push_back( t ); //   把 ] 放到 info 裡 

      } // if

      t = PeekToken();


      if ( t == "=" || t == "*=" || t == "/=" || t == "%=" || t == "+=" || t == "-=" )
      { //  assignment_operator basic_expression 

        Assignment_operator( info, p );

        



        Basic_expression( info, p );


        if ( gexe )
        {

          if ( it->variable_type != "string" &&  it->variable_type != "char" )
          {
            if ( array_pos != -1 )
            {
              value = it->value[array_pos] ;


            } // if
            else
            {
              value = it->value[0] ;

            } // else

          } // if
            
          else
          {
            if ( array_pos != -1 )
            {
              t_data = it->data[array_pos];
            } // if
            else
            {
              t_data = it->data[0];
            } // else


            

          } // else
            

        } // if



        if ( gexe )
        {




          if ( t == "=" )
          {
            if ( array_pos != -1 )
            {
              if ( it->variable_type != "string" &&  it->variable_type != "char" )
              {
                if ( it->variable_type == "int" )
                  it->value[array_pos] = ( int ) info.value[0];
                else
                  it->value[array_pos] = info.value[0];

              } // if

              else
                it->data[array_pos] = info.data[0];

            } // if
            else
            {
              if ( it->variable_type != "string" &&  it->variable_type != "char" )
              {

                if ( it->variable_type == "int" )
                  it->value[0] = ( int ) ( info.value[0] ) ;
                else
                  it->value[0] = info.value[0];

              } // if


              else
                it->data[0] = info.data[0];


            } // else


          } // if
          else if ( t == "*=" )
          {
            if ( array_pos != -1 )
            {

              if ( it->variable_type == "int" )
                it->value[array_pos] = ( int ) ( value * info.value[0] );
              else
                it->value[array_pos] = value * info.value[0];



            } // if
            else
            {
              if ( it->variable_type == "int" )
                it->value[0] = ( int ) ( value * info.value[0] ) ;
              else
                it->value[0] = value * info.value[0];



            } // else

          } // else if
          else if ( t == "/=" )
          {
            if ( array_pos != -1 )
            {
              if ( it->variable_type == "int" )
                it->value[array_pos] = ( int ) ( value / info.value[0] );
              else
                it->value[array_pos] = value / info.value[0];


            } // if
            else
            {
              if ( it->variable_type == "int" )
                it->value[0] = ( int ) ( value / info.value[0] );
              else
                it->value[0] = value / info.value[0];

            } // else

          } // else if
          else if ( t == "%=" )
          {

            int temp = info.value[0];

            if ( array_pos != -1 )
            {

              if ( it->variable_type == "int" )
                it->value[array_pos] = ( int ) ( value % temp );
              else
                it->value[array_pos] = value % temp;

            } // if

            else
            {
              if ( it->variable_type == "int" )
                it->value[0] = ( int ) ( value % temp );
              else
                it->value[0] = value % temp;

            } // else

          } // else if
          else if ( t == "+=" )
          {
            if ( array_pos != -1 )
            {
              if ( it->variable_type != "string" &&  it->variable_type != "char" )
              {
                if ( it->variable_type == "int" )
                  it->value[array_pos] = ( int ) ( value + info.value[0] ) ;
                else
                  it->value[array_pos] = value + info.value[0];

              } // if

              else
                it->data[array_pos] = t_data + info.data[0];

            } // if
            else
            {
              if ( it->variable_type != "string" &&  it->variable_type != "char" )
              {

                if ( it->variable_type == "int" )
                  it->value[0] = ( int ) ( value + info.value[0] ) ;
                else
                  it->value[0] = value + info.value[0];


              } // if

              else
                it->data[0] = t_data + info.data[0];

            } // else


          } // else if
          else  // -=
          {
            if ( array_pos != -1 )
            {
              if ( it->variable_type == "int" )
                it->value[array_pos] = ( int ) ( value - info.value[0] );
              else
                it->value[array_pos] = value - info.value[0];

            } // if
            else
            {
              if ( it->variable_type == "int" )
                it->value[0] = ( int ) ( value - info.value[0] );
              else
                it->value[0] = value - info.value[0];

            } // else



          } // else


          if ( array_pos != -1 )
          {
            if ( it->variable_type != "string" &&  it->variable_type != "char" )
              info.value[0] = it->value[array_pos];
            else
              info.data[0] = it->data[array_pos] ;

          } // if
          else
          {
            if ( it->variable_type != "string" &&  it->variable_type != "char" )
              info.value[0] = it->value[0];
            else
              info.data[0] = it->data[0] ;
          } // else





        } // if






      } // if

      else //  [ PP | MM ] romce_and_romloe  ) 
      {
        if ( t == "++" || t == "--" )
        {
          t = GetToken( true );
          info.token.push_back( t ); //   把 ++ or -- 放到 info 裡 

          if ( gexe )
          {

            if ( array_pos != -1 )
            {

              info.value[0] = it->value[array_pos];

              if ( t == "++" )
                it->value[array_pos]++;
              else
                it->value[array_pos]--;


            } // if
            else
            {
              info.value[0] = it->value[0];

              if ( t == "++" )
                it->value[0]++;
              else
                it->value[0]--;

            } // else



          } // if


        } // if

        else
        {

          if ( gexe )
          {

            if ( array_pos != -1 )
            {
              if ( it->variable_type != "string" &&  it->variable_type != "char" )
                info.value[0] = it->value[array_pos];
              else
                info.data[0] = it->data[array_pos];

            } // if
            else
            {
              if ( it->variable_type != "string" &&  it->variable_type != "char" )
                info.value[0] = it->value[0];
              else
                info.data[0] = it->data[0];

            } // else



          } // if

        } // else





        Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp( info, p );


      } // else


    } // else


  } // Rest_of_Identifier_started_basic_exp()

  void Rest_of_PPMM_Identifier_started_basic_exp( Information &info, int p )
  { //  : [ '[' expression ']' ] romce_and_romloe 
    string t, id;
    vector<Information> ::iterator it;
    int array_pos = -1;
    int value; // 紀錄原本的值
    char c = Hou_Peek();

    if ( gexe )
      id = mCorrect_Token->at( gexe_pos - 1 );




    if ( c == '[' )
    {
      t = GetToken( true ); // 讀 [
      info.token.push_back( t ); //   把 [ 放到 info 裡 
      Expression( info, p );
      t = GetToken( true ); // 讀 ]


      if ( gexe )
      {
        it = Get_variable( id );
        array_pos = info.value[0];
        if ( it->variable_type != "string" &&  it->variable_type != "char" )
        {
          if ( info.pp_mm == "++" )
            it->value[array_pos]++;
          else
            it->value[array_pos]--;

          info.value[0] = it->value[array_pos];

        } // if

        else
          info.data[0] = it->data[array_pos];


        info.variable_type = it->variable_type;


      } // if








      if ( t != "]" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); //   把 ] 放到 info 裡 


      Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp( info, p );
      return ;

    } // if


    if ( gexe )
    {
      it = Get_variable( id );

      if ( it->variable_type != "string" &&  it->variable_type != "char" )
      {
        if ( info.pp_mm == "++" )
          it->value[0]++ ;
        else
          it->value[0]-- ;

        info.value[0] = it->value[0];

      } // if

      else
      {
        info.data[0] = it->data[0];
      } // else


      info.variable_type = it->variable_type;

    } // if


    Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp( info, p );

  } // Rest_of_PPMM_Identifier_started_basic_exp()

  void Sign( Information &info, int p )
  {
    string t = GetToken( true );

    if ( t != "+" && t != "-" && t != "!" )
    {
      Vector_pop_to_original( p );
      throw SyntacticError( t );
    } // if
    
    info.sign = t ;

    info.token.push_back( t ); //   把 + or - or ! 放到 info 裡 

  } // Sign()

  void Actual_parameter_list( Information &info, int p ) // : basic_expression { ',' basic_expression }
  {
    string t;
    Basic_expression( info, p );

    while ( Hou_Peek() == ',' )
    {
      t = GetToken( true ); // 讀 ,
      info.token.push_back( t ); //   把 , 放到 info 裡 
      Basic_expression( info, p );

    } // while
  } // Actual_parameter_list()

  void Assignment_operator( Information &info, int p )
  { // : '=' | TE | DE | RE | PE | ME
    string t = GetToken( true );
    if ( t != "=" && t != "*="  && t != "/="  && t != "%="  && t != "+="  && t != "-=" )
    {
      Vector_pop_to_original( p );
      throw SyntacticError( t );
    } // if

    info.assign_type = t;
    info.token.push_back( t ); //   把  '=' | TE | DE | RE | PE | ME 放到 info 裡 

  } // Assignment_operator()

  void Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp( Information &info, int p )
  { //  rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]
    string t;
    bool conditional = false ;
    Rest_of_maybe_logical_OR_exp( info, p );

    conditional = info.value[0] ;

    if ( Hou_Peek() == '?' )
    {
      t = GetToken( true ); // 讀 ?
      info.token.push_back( t ); //   把 ? 放到 info 裡 

      if ( ! gexe )
        Basic_expression( info, p );  // 非運算階段的

      else // 運算階段
      {

        if ( conditional == true )
        {
          info.boolenop = false;
          info.variable_type = "int";
          info.sign = "+" ;
          

          Basic_expression( info, p );
          

        } // if
        else  // 跳過此文法不做
        { 
          gexe = false ;
          gexe_jump = true ;

          Basic_expression( info, p );

          gexe = true ;
          gexe_jump = false ;

        } // else

      } // else
      
     


      t = GetToken( true ); // 讀 :



      if ( t != ":" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );

      } // if

      info.token.push_back( t ); //   把 : 放到 info 裡 


      if ( ! gexe )
        Basic_expression( info, p );  // 非運算階段的

      else // 運算階段
      {

        if ( conditional == false )
        {
          info.boolenop = false;
          info.variable_type = "int";
          info.sign = "+" ;
          Basic_expression( info, p );

        } // if
        else  // 跳過此文法不做
        {
          gexe = false ;
          gexe_jump = true ;

          Basic_expression( info, p );

          gexe = true ;
          gexe_jump = false ;


        } // else

      } // else


    } // if


  } // Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp()

  void Rest_of_maybe_logical_OR_exp( Information &info, int p )
  { //  : rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }
    string t;
    int value;

    Rest_of_maybe_logical_AND_exp( info, p );


    while ( PeekToken() == "||" )
    {
      value = info.value[0];
      t = GetToken( true ); // 讀 ||
      info.token.push_back( t ); //   把 || 放到 info 裡 
      Maybe_logical_AND_exp( info, p );

      if ( gexe )
      {
        if ( info.value[0] >= 1 || value >= 1 )
          info.value[0] = 1;
        else
          info.value[0] = 0;

      } // if


      info.boolenop = true;



    } // while

  } // Rest_of_maybe_logical_OR_exp()

  void Maybe_logical_AND_exp( Information &info, int p )
  { //  : maybe_bit_OR_exp { AND maybe_bit_OR_exp }
    string t;
    int value;

    Maybe_bit_OR_exp( info, p );

    while ( PeekToken() == "&&" )
    {
      value = info.value[0];
      t = GetToken( true ); // 讀 &&
      info.token.push_back( t ); //   把 && 放到 info 裡 
      Maybe_bit_OR_exp( info, p );

      if ( gexe )
      {
        if ( info.value[0] >= 1 && value >= 1 )
          info.value[0] = 1;
        else
          info.value[0] = 0;

      } // if

      info.boolenop = true;

    } // while


  } // Maybe_logical_AND_exp()

  void Rest_of_maybe_logical_AND_exp( Information &info, int p )
  { //  : rest_of_maybe_bit_OR_exp { AND maybe_bit_OR_exp }

    string t;
    int value;

    Rest_of_maybe_bit_OR_exp( info, p );

    while ( PeekToken() == "&&" )
    {
      value = info.value[0];
      t = GetToken( true ); // 讀 &&
      info.token.push_back( t ); //   把 && 放到 info 裡 
      Maybe_bit_OR_exp( info, p );

      if ( gexe )
      {
        if ( info.value[0] >= 1 && value >= 1 )
          info.value[0] = 1;
        else
          info.value[0] = 0;

      } // if

      info.boolenop = true;

    } // while

  } // Rest_of_maybe_logical_AND_exp()

  void Maybe_bit_OR_exp( Information &info, int p )
  { // : maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }

    string t;
    Maybe_bit_ex_OR_exp( info, p );

    while ( PeekToken() == "|" )
    {
      t = GetToken( true ); // 讀 |
      info.token.push_back( t ); //   把 | 放到 info 裡 
      Maybe_bit_ex_OR_exp( info, p );

    } // while



  } // Maybe_bit_OR_exp()

  void Rest_of_maybe_bit_OR_exp( Information &info, int p )
  { // : rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }

    string t;
    Rest_of_maybe_bit_ex_OR_exp( info, p );

    while ( PeekToken() == "|" )
    {
      t = GetToken( true ); // 讀 |
      info.token.push_back( t ); //   把 | 放到 info 裡 
      Maybe_bit_ex_OR_exp( info, p );

    } // while

  } // Rest_of_maybe_bit_OR_exp()

  void Maybe_bit_ex_OR_exp( Information & info, int p )
  { // : maybe_bit_AND_exp { '^' maybe_bit_AND_exp }

    string t;
    Maybe_bit_AND_exp( info, p );
    int value;

    while ( PeekToken() == "^" )
    {
      value = info.value[0];
      t = GetToken( true ); // 讀 ^
      info.token.push_back( t ); //   把 ^ 放到 info 裡 
      Maybe_bit_AND_exp( info, p );

      /*
      if ( gexe )
      {
        int t = info.value[0];
        info.value[0] = value ^ t;

      } // if
      */

    } // while

  } // Maybe_bit_ex_OR_exp()


  void Rest_of_maybe_bit_ex_OR_exp( Information &info, int p )
  { // : rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }

    string t;
    int value;
    Rest_of_maybe_bit_AND_exp( info, p );

    while ( PeekToken() == "^" )
    {


      value = info.value[0];
      t = GetToken( true ); // 讀 ^
      info.token.push_back( t ); //   把 | 放到 info 裡 
      Maybe_bit_AND_exp( info, p );

      /*
      if ( gexe )
      {
        int t = info.value[0];
        info.value[0] = value ^ t;

      } // if
      */

    } // while


  } // Rest_of_maybe_bit_ex_OR_exp()

  void Maybe_bit_AND_exp( Information &info, int p )
  { //  : maybe_equality_exp { '&' maybe_equality_exp }

    string t;
    Maybe_equality_exp( info, p );

    while ( PeekToken() == "&" )
    {
      t = GetToken( true ); // 讀 &
      info.token.push_back( t ); //   把 & 放到 info 裡 
      Maybe_equality_exp( info, p );

    } // while


  } // Maybe_bit_AND_exp()

  void Rest_of_maybe_bit_AND_exp( Information &info, int p )
  { // : rest_of_maybe_equality_exp { '&' maybe_equality_exp }

    string t;
    Rest_of_maybe_equality_exp( info, p );

    while ( PeekToken() == "&" )
    {
      t = GetToken( true ); // 讀 &
      info.token.push_back( t ); //   把 & 放到 info 裡 
      Maybe_equality_exp( info, p );

    } // while


  } // Rest_of_maybe_bit_AND_exp()

  void Maybe_equality_exp( Information &info, int p )
  { // : maybe_relational_exp  { ( EQ | NEQ ) maybe_relational_exp  }

    string t;
    string t_data;
    int value;

    Maybe_relational_exp( info, p );




    while ( PeekToken() == "==" || PeekToken() == "!=" )
    {
      if ( gexe )
      {
        if ( info.variable_type != "string" &&  info.variable_type != "char" )
          value = info.value[0];
        else
          t_data = info.data[0];

      } // if


      t = GetToken( true ); // 讀  == 或 !=
      info.token.push_back( t ); //   把 == 或 != 放到 info 裡 
      Maybe_relational_exp( info, p );

      if ( gexe )
      {
        if ( t == "==" )
        {
          if ( info.variable_type != "string" &&  info.variable_type != "char" )
          {
            if ( info.value[0] == value )
              info.value[0] = 1;
            else
              info.value[0] = 0;
          } // if
          else
          {
            if ( info.data[0] == t_data )
              info.value[0] = 1;
            else
              info.value[0] = 0;

          } // else



        } // if
        else
        {

          if ( info.variable_type != "string" &&  info.variable_type != "char" )
          {
            if ( info.value[0] != value )
              info.value[0] = 1;
            else
              info.value[0] = 0;
          } // if
          else
          {
            if ( info.data[0] != t_data )
              info.value[0] = 1;
            else
              info.value[0] = 0;

          } // else


        } // else


      } // if

      info.boolenop = true;


    } // while


  } // Maybe_equality_exp()

  void Rest_of_maybe_equality_exp( Information &info, int p )
  { // : rest_of_maybe_relational_exp  { ( EQ | NEQ ) maybe_relational_exp }

    string t;
    int value;
    string t_data;

    Rest_of_maybe_relational_exp( info, p );


    while ( PeekToken() == "==" || PeekToken() == "!=" )
    {


      if ( gexe )
      {
        if ( info.variable_type != "string" &&  info.variable_type != "char" )
          value = info.value[0];
        else
          t_data = info.data[0];

      } // if


      t = GetToken( true ); // 讀  == 或 !=
      info.token.push_back( t ); //   把 == 或 != 放到 info 裡 
      Maybe_relational_exp( info, p );


      if ( gexe )
      {
        if ( t == "==" )
        {
          if ( info.variable_type != "string" &&  info.variable_type != "char" )
          {
            if ( info.value[0] == value )
              info.value[0] = 1;
            else
              info.value[0] = 0;
          } // if
          else
          {
            if ( info.data[0] == t_data )
              info.value[0] = 1;
            else
              info.value[0] = 0;

          } // else



        } // if
        else
        {

          if ( info.variable_type != "string" &&  info.variable_type != "char" )
          {
            if ( info.value[0] != value )
              info.value[0] = 1;
            else
              info.value[0] = 0;
          } // if
          else
          {
            if ( info.data[0] != t_data )
              info.value[0] = 1;
            else
              info.value[0] = 0;

          } // else


        } // else


      } // if


      info.boolenop = true;

    } // while


  } // Rest_of_maybe_equality_exp()

  void Maybe_relational_exp( Information &info, int p )
  { // : maybe_shift_exp    { ( '<' | '>' | LE | GE ) maybe_shift_exp }

    string t;
    int value;
    Maybe_shift_exp( info, p );


    while ( PeekToken() == "<" || PeekToken() == ">" || PeekToken() == "<=" || PeekToken() == ">=" )
    {
      value = info.value[0];
      t = GetToken( true ); // 讀  < 或 > 或 <= 或 >=
      info.token.push_back( t ); //   把 < 或 > 或 <= 或 >= 放到 info 裡 
      Maybe_shift_exp( info, p );

      if ( gexe )
      {
        if ( t == "<" )
        {
          if ( value < info.value[0] )
            info.value[0] = 1;
          else
            info.value[0] = 0;


        } // if

        else if ( t == ">" )
        {
          if ( value > info.value[0] )
            info.value[0] = 1;
          else
            info.value[0] = 0;

        } // else if

        else if ( t == "<=" )
        {
          if ( value <= info.value[0] )
            info.value[0] = 1;
          else
            info.value[0] = 0;

        } // else if

        else // >=
        {

          if ( value >= info.value[0] )
            info.value[0] = 1;
          else
            info.value[0] = 0;

        } // else

        info.boolenop = true;



      } // if



    } // while


  } // Maybe_relational_exp()

  void Rest_of_maybe_relational_exp( Information &info, int p )
  { // : rest_of_maybe_shift_exp    { ( '<' | '>' | LE | GE ) maybe_shift_exp }

    string t;
    int value;
    Rest_of_maybe_shift_exp( info, p );

    while ( PeekToken() == "<" || PeekToken() == ">" || PeekToken() == "<=" || PeekToken() == ">=" )
    {
      value = info.value[0];
      t = GetToken( true ); // 讀  < 或 > 或 <= 或 >=
      info.token.push_back( t ); //   把 < 或 > 或 <= 或 >= 放到 info 裡 
      Maybe_shift_exp( info, p );

      if ( gexe )
      {
        if ( t == "<" )
        {
          if ( value < info.value[0] )
            info.value[0] = 1;
          else
            info.value[0] = 0;


        } // if

        else if ( t == ">" )
        {
          if ( value > info.value[0] )
            info.value[0] = 1;
          else
            info.value[0] = 0;

        } // else if

        else if ( t == "<=" )
        {
          if ( value <= info.value[0] )
            info.value[0] = 1;
          else
            info.value[0] = 0;

        } // else if

        else // >=
        {

          if ( value >= info.value[0] )
            info.value[0] = 1;
          else
            info.value[0] = 0;

        } // else

        info.boolenop = true;



      } // if


    } // while


  } // Rest_of_maybe_relational_exp()


  void Maybe_shift_exp( Information & info, int p )
  { // : maybe_additive_exp { ( LS | RS ) maybe_additive_exp }

    string t;
    int t_value;
    Maybe_additive_exp( info, p );

    while ( PeekToken() == "<<" || PeekToken() == ">>" )
    {

      t_value = info.value[0];

      t = GetToken( true ); // 讀 << 或 >>
      info.token.push_back( t ); //   把 << 或 >> 放到 info 裡 
      Maybe_additive_exp( info, p );

      if ( gexe )
      {
        int tt = info.value[0];

        if ( t == "<<" )
          info.value[0] = t_value << tt;

        else
          info.value[0] = t_value >> tt;

      } // if

    } // while

  } // Maybe_shift_exp()

  void Rest_of_maybe_shift_exp( Information &info, int p )
  { // : rest_of_maybe_additive_exp { ( LS | RS ) maybe_additive_exp }

    string t;
    int t_value;
    Rest_of_maybe_additive_exp( info, p );

    while ( PeekToken() == "<<" || PeekToken() == ">>" )
    {
      t_value = info.value[0];
      t = GetToken( true ); // 讀 << 或 >>
      info.token.push_back( t ); //   把 << 或 >> 放到 info 裡 
      Maybe_additive_exp( info, p );

      if ( gexe )
      {
        int tt = info.value[0];

        if ( t == "<<" )
          info.value[0] = t_value << tt;

        else
          info.value[0] = t_value >> tt;

      } // if

    } // while


  } // Rest_of_maybe_shift_exp()

  void Maybe_additive_exp( Information &info, int p )
  { // : maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }

    string t;
    string t_data; // 暫存用不重要
    int value; // 暫存用不重要



    Maybe_mult_exp( info, p );

    while ( PeekToken() == "+" || PeekToken() == "-" )
    {
      if ( gexe )
      {
        if ( info.variable_type != "string" &&  info.variable_type != "char" )
          value = info.value[0];
        else
          t_data = info.data[0];

      } // if



      t = GetToken( true ); // 讀 + 或 -
      info.token.push_back( t ); //   把 + 或 - 放到 info 裡 
      Maybe_mult_exp( info, p );

      if ( gexe )
      {
        if ( info.variable_type != "string" &&  info.variable_type != "char" )
        {
          if ( t == "+" )
            info.value[0] = value + info.value[0];
          else
            info.value[0] = value - info.value[0];

        } // if
        else
        {

          if ( t == "+" )
            info.data[0] = t_data + info.data[0];
          else
            cout << "no way!" << endl;

        } // else 



      } // if



    } // while


  } // Maybe_additive_exp()

  void Rest_of_maybe_additive_exp( Information &info, int p )
  { // : rest_of_maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }

    string t, t_data;

    int value;
    Rest_of_maybe_mult_exp( info, p );

    while ( PeekToken() == "+" || PeekToken() == "-" )
    {

      if ( gexe )
      {
        if ( info.variable_type != "string" &&  info.variable_type != "char" )
          value = info.value[0];
        else
          t_data = info.data[0];

      } // if


      t = GetToken( true ); // 讀 + 或 -
      info.token.push_back( t ); //   把 + 或 - 放到 info 裡 
      Maybe_mult_exp( info, p );


      if ( gexe )
      {
        if ( info.variable_type != "string" &&  info.variable_type != "char" )
        {
          if ( t == "+" )
            info.value[0] = value + info.value[0];
          else
            info.value[0] = value - info.value[0];

        } // if
        else
        {

          if ( t == "+" )
            info.data[0] = t_data + info.data[0];
          else
            cout << "no way" << endl;

        } // else 



      } // if

    } // while


  } // Rest_of_maybe_additive_exp()

  void Maybe_mult_exp( Information &info, int p )
  { // : unary_exp rest_of_maybe_mult_exp

    Unary_exp( info, p );
    Rest_of_maybe_mult_exp( info, p );

  } // Maybe_mult_exp()

  void Rest_of_maybe_mult_exp( Information &info, int p )
  { //  : { ( '*' | '/' | '%' ) unary_exp }  /* could be empty ! */

    string t;

    int value;
    double value2 ;

    bool pre_is_float = false  ;

    while ( PeekToken() == "*" || PeekToken() == "/" || PeekToken() == "%" )
    {
      value = info.value[0];
      value2 = info.value[0] ;

      pre_is_float = info.isFloat ;

      t = GetToken( true ); // 讀 * 或 / 或 %
      info.token.push_back( t ); //   把 * 或 / 或 % 放到 info 裡 
      Unary_exp( info, p );


      if ( gexe )
      {
        if ( t == "*" )
          info.value[0] = value2 * info.value[0];
        else if ( t == "/" )
        {
          if ( pre_is_float || info.isFloat )
            info.value[0] = value2 / info.value[0];
          else
            info.value[0] = ( int ) value2 / ( int ) info.value[0] ;


        } // else if
         
        else
        {
          int temp = info.value[0];
          info.value[0] = value % temp;

         

        } // else 

      } // if





    } // while



  } // Rest_of_maybe_mult_exp()

  void Unary_exp( Information &info, int p )
  { // : sign { sign } signed_unary_exp  | unsigned_unary_exp | ( PP | MM ) Identifier [ '[' expression ']']
    string t = PeekToken();
    vector<Information> ::iterator it;
    int array_pos = -1;
    string sign_temp ;


    if ( t == "+" || t == "-" || t == "!" )
    {
      Sign( info, p );
      
      sign_temp = info.sign ;
      
      while ( PeekToken() == "+" || PeekToken() == "-" || PeekToken() == "!" )
      {
        Sign( info, p );

        if ( sign_temp == info.sign )
          info.sign = "+" ;

        sign_temp = info.sign ;

      } // while

      Signed_unary_exp( info, p );

    } // if
    else if ( t == "++" || t == "--" )
    {
      string op, id;
      t = GetToken( true ); // 讀 ++ 或 --
      op = t;


      info.token.push_back( t ); //   把 ++ 或 -- 放到 info 裡 
      t = GetToken( true ); // 讀 id
      id = t;

      if ( !Is_Identifier( t ) )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      if ( !Is_definitioned( t ) )
      {
        Vector_pop_to_original( p );
        throw Undefined_identifier( t );
      } // if

      info.token.push_back( t ); //   把 id 放到 info 裡 


      if ( Hou_Peek() == '[' )
      {
        t = GetToken( true ); // 讀 [
        info.token.push_back( t ); //   把 [ 放到 info 裡 
        Expression( info, p );

        if ( gexe )
        {
          it = Get_variable( id );

          array_pos = info.value[0];

          if ( it->variable_type != "string" &&  it->variable_type != "char" )
          {
            if ( op == "++" )
              it->value[array_pos]++;
            else
              it->value[array_pos]--;

            info.value[0] = it->value[array_pos];

          } // if

          else
            info.data[0] = it->data[array_pos];


          info.variable_type = it->variable_type;


        } // if



        t = GetToken( true ); // 讀 ]
        if ( t != "]" )
        {
          Vector_pop_to_original( p );
          throw SyntacticError( t );

        } // if

        info.token.push_back( t ); //   把 ] 放到 info 裡 

        return ;

      } // if

      if ( gexe )
      {
        it = Get_variable( id );

        if ( it->variable_type != "string" &&  it->variable_type != "char" )
        {
          if ( op == "++" )
            it->value[0]++ ;
          else
            it->value[0]-- ;

          info.value[0] = it->value[0];

        } // if

        else
        {
          info.data[0] = it->data[0];
        } // else


        info.variable_type = it->variable_type;

      } // if






    } // else if 

    else
      Unsigned_unary_exp( info, p );


  } // Unary_exp()

  void Signed_unary_exp( Information &info, int p )
  { //  : Identifier [  '(' [ actual_parameter_list ] ')'  |   '[' expression ']'  ]
    //    | Constant | '(' expression ')'
    vector<Information> ::iterator it;
    string t = GetToken( true );
    string id;

    int array_pos = -1;
    char c;
    int tLine = gLine;

    if ( Is_Identifier( t ) )
    {
      if ( Hou_Peek() == '(' )  // 代表此id是function
      {

        if ( !Is_definitioned( t + "()" ) )
        {
          if ( gLine > tLine )
            g_control = true;

          gpreLine = gLine - tLine;
          gLine = tLine;
          Vector_pop_to_original( p );
          throw Undefined_identifier( t );
        } // if

      } // if
      else
      {

        if ( !Is_definitioned( t ) )
        {

          if ( gLine > tLine )
            g_control = true;

          gpreLine = gLine - tLine;
          gLine = tLine;
          Vector_pop_to_original( p );
          throw Undefined_identifier( t );
        } // if

      } // else

      info.token.push_back( t ); //   把 id 放到 info 裡 
      id = t;
      c = Hou_Peek();

      if ( gexe )
      {
        it = Get_variable( id );
        info.value[0] = it->value[0];
        info.variable_type = it->variable_type;

        if ( it->variable_type == "bool" )
          info.boolenop = true ;

        if ( info.sign == "-" )
        {
          info.value[0] *= -1;
        } // if

        if ( info.sign == "!" )
        {
          if ( info.value[0] <= 0 )
            info.value[0] = 1;
          else
            info.value[0] = 0;

          info.sign = "+" ;

        } // if


      } // if


      if ( c == '[' || c == '(' )
      {
        t = GetToken( true ); // 讀 [ 或 (
        info.token.push_back( t ); //   把 [ 或 ( 放到 info 裡 

        if ( t == "[" ) // '[' expression ']'
        {

          Expression( info, p );

          if ( gexe )
          {
            it = Get_variable( id );
            array_pos = info.value[0];

            info.value[0] = it->value[array_pos];

            if ( info.sign == "!" )
            {
              if ( info.value[0] <= 0 )
                info.value[0] = 1;
              else
                info.value[0] = 0;

              info.sign = "+" ;

            } // if


            if ( info.sign == "-" )
            {
              info.value[0] *= -1;
            } // if

            



          } // if




          t = GetToken( true ); // 讀 ] 或 )

          if ( t != "]" )
          {
            Vector_pop_to_original( p );
            throw SyntacticError( t );

          } // if


        } // if


        else // '(' [ actual_parameter_list ] ')'
        {
          if ( Hou_Peek() != ')' )
            Actual_parameter_list( info, p );

          t = GetToken( true ); // 讀 ] 或 )

          if ( t != ")" )
          {
            Vector_pop_to_original( p );
            throw SyntacticError( t );

          } // if

        } // else




        info.token.push_back( t ); //   把 ] 或 ) 放到 info 裡 


      } // if


    } // if

    else if ( Is_Constant( t ) )
    {
      info.token.push_back( t ); //   把 constant 放到 info 裡 
      if ( gexe )
      {
        if ( t == "true" )
        {

          info.value[0] = 1;
          info.boolenop = true;

        } // if
        else if ( t == "false" )
        {
          info.value[0] = 0;
          info.boolenop = true;

        } // else if
        else
        {
          if ( String_Find_the_char( t, '.' ) )
            info.isFloat = true;

          if ( info.isFloat )
            info.value[0] = atof( t.c_str() ) ;

          else
            info.value[0] = atoi( t.c_str() );

        } // else



        if ( info.sign == "!" )
        {
          if ( info.value[0] <= 0 )
            info.value[0] = 1;
          else
            info.value[0] = 0;

          info.sign = "+" ;

        } // if

        if ( info.sign == "-" )
        {
          info.value[0] *= -1;
        } // if


      } // if


    } // else if


    else if ( t == "(" )
    {
      string sign_temp = info.sign ;

      info.token.push_back( t ); //   把 ( 放到 info 裡 
      Expression( info, p );
      t = GetToken( true ); // 讀 )
      if ( t != ")" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); //   把 ) 放到 info 裡

      if ( gexe )
      {
       

        if ( sign_temp == "!" )
        {
          if ( info.value[0] <= 0 )
            info.value[0] = 1;
          else
            info.value[0] = 0;


        } // if

        if ( sign_temp == "-" )
        {
          info.value[0] *= -1;
        } // if


      } // if



    } // else if

    else {
      Vector_pop_to_original( p );
      throw SyntacticError( t );

    } // else


  } // Signed_unary_exp()

  void Unsigned_unary_exp( Information &info, int p )
  {
    vector<Information> ::iterator it;

    string id;
    string t = GetToken( true );

    int array_pos = -1;
    char c;
    int tLine = gLine;

    if ( Is_Identifier( t ) )
    {


      if ( Hou_Peek() == '(' )  // 代表此id是function
      {

        if ( !Is_definitioned( t + "()" ) )
        {
          if ( gLine > tLine )
            g_control = true;

          gpreLine = gLine - tLine;
          gLine = tLine;
          Vector_pop_to_original( p );
          throw Undefined_identifier( t );
        } // if

      } // if
      else
      {

        if ( !Is_definitioned( t ) )
        {

          if ( gLine > tLine )
            g_control = true;

          gpreLine = gLine - tLine;
          gLine = tLine;
          Vector_pop_to_original( p );
          throw Undefined_identifier( t );
        } // if

      } // else

      info.token.push_back( t ); //   把 id 放到 info 裡 
      id = t;

      t = PeekToken();

      if ( gexe )
      {
        it = Get_variable( id );

        if ( it->variable_type != "string" &&  it->variable_type != "char" )
          info.value[0] = it->value[0];
        else
          info.data[0] = it->data[0] ;


        info.variable_type = it->variable_type ;

        if ( it->variable_type == "bool" )
          info.boolenop = true ;


      } // if



      if ( t == "[" || t == "(" )
      {
        t = GetToken( true ); // 讀 [ 或 (
        info.token.push_back( t ); //   把 [ 或 ( 放到 info 裡 

        if ( t == "[" ) // '[' expression ']'
        {

          Expression( info, p );

          if ( gexe )
          {
            it = Get_variable( id );
            array_pos = info.value[0];


            if ( it->variable_type != "string" &&  it->variable_type != "char" &&
                 it->variable_type != "char" )
              info.value[0] = it->value[array_pos];
            else
            {
              info.data[0] = it->data[array_pos];
              info.variable_type = "string" ;

            } // else
              

          } // if



          t = GetToken( true ); // 讀 ] 或 )

          if ( t != "]" )
          {
            Vector_pop_to_original( p );
            throw SyntacticError( t );

          } // if

          info.token.push_back( t ); //   把 ] 或 ) 放到 info 裡 

          t = PeekToken();
          


          if ( t == "++" || t == "--" )
          {
            t = GetToken( true );
            info.token.push_back( t ); //   把 ] 或 ) 放到 info 裡 

            if ( gexe )
            {

              if ( t == "++" )
              {
                info.value[0] = it->value[array_pos];
                it->value[array_pos]++;
                

              } // if
              else
              {
                info.value[0] = it->value[array_pos];
                it->value[array_pos]--;
               

              } // else

              // info.value[0] = it->value[array_pos];

            } // if



          } // if

          return;

        } // if


        else // '(' [ actual_parameter_list ] ')'
        {


          if ( Hou_Peek() != ')' )
            Actual_parameter_list( info, p );


          t = GetToken( true ); // 讀 ] 或 )

          if ( t != ")" )
          {
            Vector_pop_to_original( p );
            throw SyntacticError( t );

          } // if

          info.token.push_back( t ); //   把 ] 或 ) 放到 info 裡 



        } // else

        return;

      } // if




      if ( t == "++" || t == "--" )
      {
        t = GetToken( true ); // 讀入 ++ 或 --
        info.token.push_back( t ); //   把 ++ 或 -- 放到 info 裡 



        if ( gexe )
        {
          it = Get_variable( id );

          if ( it->variable_type != "string" &&  it->variable_type != "char" )
          {

            info.value[0] = it->value[0];

            if ( t == "++" )
              it->value[0]++ ;
            else
              it->value[0]-- ;

           

          } // if

          else
          {
            info.data[0] = it->data[0];
          } // else


          info.variable_type = it->variable_type;

        } // if





      } // if





    } // if

    else if ( Is_Constant( t ) )
    {
      info.token.push_back( t ); //   把 constant 放到 info 裡 

      if ( gexe )
      {
        if ( t == "true" )
        {

          info.value[0] = 1;
          info.boolenop = true;
          info.variable_type = "int";

        } // if
        else if ( t == "false" )
        {
          info.value[0] = 0;
          info.boolenop = true;
          info.variable_type = "int";

        } // else if
        else if ( Is_Num( t ) )
        {

          if ( String_Find_the_char( t, '.' ) )
            info.isFloat = true;

          if ( info.isFloat )
            info.value[0] = atof( t.c_str() ) ;

          else
            info.value[0] = atoi( t.c_str() );

          info.variable_type = "int";

        } // else if
        else
        {
          info.variable_type = "string";

          if ( t != "\"\\n\"" )
            info.data[0] = t.assign( t, 1, t.size() - 2 );
          else
            info.data[0] = "\n";




        } // else

      } // if
    } // else if


    else if ( t == "(" )
    {
      info.token.push_back( t ); //   把 ( 放到 info 裡 
      Expression( info, p );
      t = GetToken( true ); // 讀 )
      if ( t != ")" )
      {
        Vector_pop_to_original( p );
        throw SyntacticError( t );
      } // if

      info.token.push_back( t ); //   把 ) 放到 info 裡


    } // else if

    else {
      Vector_pop_to_original( p );
      throw SyntacticError( t );

    } // else


  } // Unsigned_unary_exp()


};


int main()
{
 


  Data *data = new Data();
  

  char temp[180]; //  用來getline用的 不太重要
  bool run = true;             // string a = "+3.5";
  char testnum = '\0';

  /*
  Information a;
  a.token.push_back( "5" );
  vector<string> * t;
  t = new vector<string>();
  t->insert( t->end(), a.token.begin(), a.token.end() );
  cout << t->at( 0 );
  */

  cin.getline( temp, 180 );
  testnum = temp[0];
  
  
  // if ( testnum == '2' )
  //  cout << "123";


  cout << "Our-C running ..." << endl;
  // cout << "start: " << data.Is_Num('.') << endl;
  bool endLine_is_road = false;


  while ( run ) //  用無窮迴圈是因為 QUIT也打算以throw的方式來處理
  {
    try {

      if ( gexe )
        cout << "> ";

      data->User_input();


    }
    catch ( LexicalError e )
    {
      // cout << e.mError_Message;
      cin.getline( temp, 180 );
      endLine_is_road = true;

      // if ( testnum == '3' )
      //  cout << temp[181];


      data->Vector_pop_to_original( gpos );

      data->mCorrect_Token->push_back( "cout" );
      data->mCorrect_Token->push_back( "<<" );
      data->mCorrect_Token->push_back( "\"" + e.mError_Message + "\"" );
      
      data->mCorrect_Token->push_back( ";" );

    } // catch
    catch ( SyntacticError e )
    {
      // cout << e.mError_Message;
      cin.getline( temp, 180 );
      endLine_is_road = true;

      // if ( testnum == '3' )
      //  cout << temp[181];

      data->Vector_pop_to_original( gpos );

      data->mCorrect_Token->push_back( "cout" );
      data->mCorrect_Token->push_back( "<<" );
      data->mCorrect_Token->push_back( "\"" + e.mError_Message + "\"" );
     
      data->mCorrect_Token->push_back( ";" );


    } // catch
    catch ( Undefined_identifier e )
    {
    
      // if ( testnum == '3' )
      //  cout << temp[181];

      // cout << e.mError_Message;
      if ( !g_control )
      {
        cin.getline( temp, 180 );
        endLine_is_road = true;
      } // if

      data->Vector_pop_to_original( gpos );


      data->mCorrect_Token->push_back( "cout" );
      data->mCorrect_Token->push_back( "<<" );
      data->mCorrect_Token->push_back( "\"" + e.mError_Message + "\"" );
      data->mCorrect_Token->push_back( ";" );


    } // catch
    catch ( Divid_Zero e )
    {

      if ( testnum == '3' )
        cout << temp[181];

      cout << e.mError_Message;
      cin.getline( temp, 180 );
      endLine_is_road = true;
    } // catch
    catch ( End e )
    {



      if ( gexe )
        cout << e.mError_Message;

      if ( !gexe )
        cin.getline( temp, 180 );

      endLine_is_road = true;


      if ( !gexe )
      {

        data->mInformation_table.clear();
        data->mVariableList->clear();

      } // if

      if ( gexe )
        run = false;

      gexe = true;

    } // catch

    if ( endLine_is_road )
      gLine = 1;

    else
    {
      if ( run && !gexe )
        data->Remove_One_Line_White_Space();

      gLine = 1;

    } // else


    endLine_is_road = false;

    //  try {

    //  if ( run )
    //  {
    //    cout << "> ";
    //    data->PeekToken();
    //  } // if
    //  }
    //  catch ( LexicalError e )
    //  {
    //  cout << e.mError_Message;
    //  cin.getline( temp, 128 );
    //  endLine_is_road = true;
    // } // catch
    //
    // if ( endLine_is_road )
    //  gLine = 1;

    if ( gpreLine != 0 )
      gLine = gpreLine;


    gpreLine = 0;
    g_control = false;
    gpos_no_use = false;

  } // while


    // cout << data->GetToken( true  );


  // system( "pause" );

} // main()

