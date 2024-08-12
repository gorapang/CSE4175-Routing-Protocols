# Computer Networks Machine Problem 2: Routing Protocols 

네트워크 라우팅의 두 가지 알고리즘 **Distance Vector**와 **Link State**을 구현했습니다. 각 알고리즘은 네트워크 환경에서 각 라우터의 라우팅 테이블을 생성하고, 네트워크 토폴로지의 변화에 따라 이를 업데이트하는 시뮬레이션을 수행합니다.

## 프로젝트 개요


1. **`distvec_20200185.cc`**: Distance Vector 라우팅 알고리즘을 시뮬레이션하며, 각 라우터는 네트워크의 다른 모든 라우터까지의 최소 거리를 벡터로 유지하고, 이웃 라우터로부터 받은 정보를 바탕으로 라우팅 테이블을 업데이트합니다.

2. **`linkstate_20200185.cc`**: Link State 라우팅 알고리즘을 시뮬레이션하며, 각 라우터는 네트워크 토폴로지의 전체 정보를 가지고 Dijkstra 알고리즘을 사용해 다른 모든 라우터까지의 최단 경로를 계산합니다.

<br>

## 컴파일 방법
```bash
g++ -o distvec distvec_20200185.cc
g++ -o linkstate linkstate_20200185.cc
```

<br>

## 실행 방법


### 명령어 사용법

Distance Vector 알고리즘 실행:
```bash
./distvec topologyfile messagesfile changesfile
```

Link State 알고리즘 실행:

```bash
./linkstate topologyfile messagesfile changesfile
```

### 실행 예시
```bash
./distvec topology.txt messages.txt changes.txt
./linkstate topology.txt messages.txt changes.txt
```

### 입력 파일
`topology.txt`: 초기 네트워크 구조 정의

`messages.txt`: 한 노드에서 다른 노드로 라우팅할 메시지 정의

`changes.txt`: 링크 추가 및 제거와 같은 네트워크 토폴로지의 변경 사항 정의


### 출력
`distvec` 프로그램은 output_dv.txt 파일에 결과를 출력

`linkstate` 프로그램은 output_ls.txt 파일에 결과를 출력


출력 파일 내용:
```
토폴로지 파일을 처리한 후의 라우팅 테이블.

네트워크 변경 전후 각 메시지의 라우팅 결과.

네트워크 변경이 적용된 후 업데이트된 라우팅 테이블과 메시지 라우팅 결과.
```
