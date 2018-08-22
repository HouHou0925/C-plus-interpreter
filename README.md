
PL3(CYCU PL課程projcet)  
使用語言:C++  
編譯器: vs 2017  
(中原資工系同學請勿Copy 後果自負)
===========================

****
	
|Author|侯昱宏|
|---|---
|E-mail|h55334147879@gmail.com


****
## 目錄
* [介紹](#介紹)
* [功能](#功能)
* [範例(基本功能)](#基本功能範例)


------------------------------------------------------  

介紹
----
此程式為中原大學三下PL(程式語言)project
占分70% 分為 project1~4  
Project1為 基礎的運算能力及變數宣告  
Project2為 C語言文法的check  
Project3為 2的進階 除了確認文法的正確性還要加上運算能力及輸出答案  
Porject4為 3的進階 要加入Call function的能力  
此分享程式只做到project3 因為時間的關係所以沒繼續往下寫


------------------------------------------------------

功能
----  
1.可以根據輸入的字串 判斷是否為正確的C語言文法    
基本上大部分的C文法都能接受  
2.程式開始前要先隨便輸入一個數字 然後按enter後才算開始執行  
3.程式結束以輸入 Done();  為結束  
4.可接受 變數宣告.function宣告.陣列宣告及運算.條件式.迴圈等等運算能力
5.提供 ListVariable("變數名稱")、ListAllVariables()、
       ListFunction("名稱")、ListAllFunctions() 等4個額外輸入  
6.輸入錯誤時會印出對印的錯誤行數  
7. Error可以分出是lexical error(不接受的token @#$等等)、syntactic error(文法錯誤)、 semantic error(呼叫未宣告變數)  
8.輸出部分除了第5點所提及的功能及error訊息外 皆在Done(); 之後輸出  
因為做法事 採用先檢查一遍文法正確 將正確文法存下後 第二遍才跑運算的部分  
而不是一邊檢查文法一邊運算

------------------------------------------------------

基本功能範例
----
範例1: 有做function的call stack 可以區分出 變數的範圍  
Done(); 以上的為輸入 以下為輸出
![image](https://github.com/silence0925/PL/blob/master/pl%E7%AF%84%E4%BE%8B1.PNG)

範例2:Error示範 (行數的部分 每一次跑parse tree會重置)

![image](https://github.com/silence0925/PL/blob/master/%E9%8C%AF%E8%AA%A4%E7%A4%BA%E7%AF%84.PNG)

範例3: 4個function示範  

![image](https://github.com/silence0925/PL/blob/master/4%E5%80%8Bfunction.PNG)









