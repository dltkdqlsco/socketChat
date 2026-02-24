# socketChat
socket를 활용한 1대다 채팅  

통신을 공부하면서 채팅을 구현해보고 싶어서 
https://blog.naver.com/sunbei00/222205589249 이분의 블로그와 gemini의 도움으로 socket를 활용해서 채팅을 구현을 해보았습니다.

server socket과 client socket를 통해 데이터 통로를 만들고 서로 통신을 하는 법을 어떻게 구현하는지 조금이나마 알게되었다.

그리고 블로그에 있는 코드는 window 적합한 코드여서 macOS에 맞는 코드로 바꿔서 구현을 하였다.
이 프로젝트를 하면서 thread의 join과 detach의 차이를 확실하게 할게 되었다.
짧게 말하자면 
  join : main이 스레드가 끝날때까지 기다리는것이고
  detach : main이 스레드가 끝나던 말던 지 할일을 하는것이다.
