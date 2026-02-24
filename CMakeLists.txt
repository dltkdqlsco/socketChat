# CMake 빌드 시스템의 최소 요구 버전을 설정한다.
cmake_minimum_required(VERSION 3.20)
# 프로젝트 이름을 'TerminalChat'으로 정하고, 사용하는 언어는 C++(CXX)임을 선언한다.
project(TerminalChat LANGUAGES CXX)

# C++ 표준을 17버전으로 설정한다.
# std::thread, std::mutex등 최신 기능을 쓰기 위함이다.
set(CMAKE_CXX_STANDARD 17)
# 설정한 C++ 버전(17)이 지원되지 않는 환경이라면 에러를 발생시켜 빌드를 중단한다.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 시스템에서 멀티스레드 라이브러리(macOS/Linux의 경우 pthread 등)를 찾아온다.
# 이 설정이 있어야 std::thread 관련 코드가 문제없이 링크된다.
find_package(Threads REQUIRED)

# ----- 서버 실행 파일 설정 -----
# server_main.cpp를 사용해 server 실행 파일 생성
add_executable(server server/server_main.cpp)
# 스레드 라이브러리 연결
target_link_libraries(server PRIVATE Threads::Threads)


# ----- 클라이언트 실행 파일 설정 -----
# client_main.cpp를 사용해 client 실행 파일 생성
add_executable(client client/client_main.cpp)
# 스레드 라이브러리 연결
target_link_libraries(client PRIVATE Threads::Threads)

