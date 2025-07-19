# GameServer & DBServer Project

본 프로젝트는 IOCP 기반의 멀티스레딩 서버 아키텍처로,
GameServer와 DBServer가 TCP 기반으로 통신하며 비동기처럼 동작하도록 설계되었습니다.
GameServer는 Zone 기반 TaskQueue를 통해 게임 로직을 병렬 처리하고,
DBServer는 playerId 기반 요청 샤딩 및 전용 워커 스레드를 통해 DB 요청을 효율적으로 처리합니다.
또한, 커스텀 메모리 풀, CMS 자동화 등 실제 게임 서버 환경을 고려한 구조를 적용하였습니다.

📄 상세한 구현 내역 및 설명은 `description/포트폴리오.pdf` 문서를 참고해주세요.
