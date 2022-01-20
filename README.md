# Network Study

학습 기간: 2021.03.02 ~ 2021.03.07

목적: [codesafe의 CamScannerController](https://github.com/codesafe/CamScannerController)에 쓰인 TCP 서버&클라이언트를 바탕으로 분석 및 학습하며 C++로 나만의 게임 서버&클라이언트를 제작!

모방하며 기술 스택들과 클래스 설계 및 C++ 스타일을 학습 가능

### LOGGER의 기능

Logger는 진행되는 것들을 파일로 저장 혹은 콘솔에 기록해둔다.



### MemDB의 기능

Memory DB라고 해서 유니티 내에는 레지스트리에 저장되는 PlayerPref이 있는 것을 씬 전환시에도 저장하고 싶은 정보들을 담는 래퍼 클래스다. 내부는 std::map으로 구현돼있다. 



#### 진행 로그 2021.03.06.

통신이 되는 서버와 클라이언트를 만들었다. 도무지 내 온전한 힘으로는 되지 않아서 [여러가지(C++ Event driven TCP Socket Server)](https://gist.github.com/Gydo194/c14e52701289354ab66359c2a75706f8)를 살펴봐야만 했다. 전에 했던 [Project-0 시리즈](https://github.com/PioneerRedwood/Project-02)들도 도움이 됐다. 아직 최종적인 서버 클라이언트 클래스가 완성이 안됐기 때문에 손을 봐야한다. 그러고 난 후에 유니티와 연동해 만들어야겠다. 그 [다음 프로젝트](https://github.com/PioneerRedwood/GameNetwork)는 public으로 한다

TODO

- 서버/클라이언트 클래스
- 다중 클라이언트 접속
- 네트워킹 스레드 설계



# New Start #2

### boost::asio network

since. 2021.03.30

@copyright [OneLoneCoder's Youtube](https://www.youtube.com/c/javidx9), [OneLoneCoder's GitHub](https://github.com/OneLoneCoder)

