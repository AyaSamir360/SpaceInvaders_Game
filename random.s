
       THUMB
       AREA    DATA, ALIGN=2
M      SPACE   4
       ALIGN          
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Random_Init
       EXPORT  Random
Random_Init
       LDR R2,=M       
       MOV R0,#1       
       STR R0,[R2]    
       BX  LR

Random LDR R2,=M   
       LDR R0,[R2] 
       LDR R1,=1664525
       MUL R0,R0,R1 
       LDR R1,=1013904223
       ADD R0,R1     
       STR R0,[R2]  
       BX  LR
       ALIGN      
       END  