# Network Study [Private]

학습 기간: 2021.03.02 ~ 

목적: [codesafe의 CamScannerController](https://github.com/codesafe/CamScannerController)에 쓰인 TCP 서버&클라이언트를 바탕으로 분석 및 학습하며 C++로 나만의 게임 서버&클라이언트를 제작!

모방하며 기술 스택들과 클래스 설계 및 C++ 스타일을 학습 가능

### LOGGER의 기능

Logger는 진행되는 것들을 파일로 저장 혹은 콘솔에 기록해둔다.



### MemDB의 기능

Memory DB라고 해서 유니티 내에는 레지스트리에 저장되는 PlayerPref이 있는 것을 씬 전환시에도 저장하고 싶은 정보들을 담는 래퍼 클래스다. 내부는 std::map으로 구현돼있다. 

