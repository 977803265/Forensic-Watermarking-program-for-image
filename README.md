# Forensic-Watermarking-program-for-image

## Development contents
   **이미지 기반의 다양한 창작물들에 대하여 불법 복제와 유통이 성행하는 현재, 기존의 워터마크 활용 방식을 확장하여 이를 추적하는데 기여하는 포렌식 워터마킹 프로그램 제작을 목표로 한다.**    

**해당 프로젝트는 삽입할 데이터를 입력받고 QR 코드로 변환한 후 선택 버튼을 통해 삽입하고자 하는 원본 이미지를 선택하고 삽입 버튼으로 삽입 과정을 진행한다. 추출 버튼을 통해 워터마크가 삽입된 이미지를 선택하여 추출을 진행하여 삽입된 QR 코드를 복원하고 QR 코드 스캐너로 삽입된 정보를 확인한다.**   

* **압축 강인성**: 워터마크가 삽입된 이미지를 고의적으로 변형을 가하여 실효성을 제거하려 하는 공격들이 존재하는데, 해당 프로젝트는 삽입된 이미지에 압축 공격을 가하는 방식에 대한 강인성을 확보한다. 일반적으로 널리 사용되는 JPEG 압축 방식을 이용한 손실 압축에도 삽입된 워터마크가 유지되는 압축 강인성을 가지도록 구현한다.

* **비가시성**: 삽입된 워터마크가 원본 이미지에 그대로 나타나는 삽입 방식과 원본 이미지에 나타나지 않는 방식으로 삽입 알고리즘을 구현할 수 있다. 해당 프로젝트는 삽입된 워터마크가 원본 이미지에 표현되지 않도록 구현하여 비가시성을 확보한다. 또한, 워터마크가 삽입된 이미지에서 워터마크를 추출하는 과정에서 원본 이미지가 필요하지 않다. 따라서, 해당 프로젝트는 blind watermarking으로 구별되어진다.

* **QR 코드**: 해당 프로젝트에서 사용되는 워터마크는 입력 받은 정보를 버전1~3의 QR 코드로 변환하여 사용한다. 삽입하고자 입력받는 데이터의 길이에 따라 최대 32x32 크기의 QR 코드를 생성하며 352bit(숫자 101, 영숫자 61, binary 42, 한자 26의 길)를 표현할 수 있다. 또한, 오류복원 단계 M을 기준으로 15%의 손상 부분을 자체적으로 복원하는 기능을 가진다. 이러한 이유로 해당 프로젝트의 워터마크로 사용된다.    

## Development enviroment
**개발 도구**: VisualStudio 2017, HeidiSQL    
**개발 언어**: C++, PHP, HTML   
**사용 라이브러리**: MFC, OpenCV   
* 삽입하고자 하는 데이터 입력, 원본 이미지 선택, 삽입, 추출의 과정을 위한 GUI 프로그램 개발을 위해 MFC 기반으로 개발한다.
* 디지털 이미지에 대한 다양한 영상처리 기술을 이용하기 위해 오픈소스 라이브러리인 OpenCV를 이용한다.   

**개발 인원**: 5명(팀 프로젝트)   
* **(담당 부분)삽입/추출 알고리즘 구현 2명**, **결과 검증을 위한 정량적 수치 도출 1명**   
이미지 다운로드 웹 사이트 개발과 워터마킹 프로그램 호환 2명, 워터마크 생성 담당(QR code) 1명   

**개발 기간**: 2018년 11월 ~ 2019년 4월

## Implement
**QR code**   
* 오픈소스를 이용하여 입력 정보(문자열)를 QR code로 변환한다. 삽입 알고리즘 구현에서의 JPEG 압축 방식, DWT의 다운 샘플링 등을 고려하여 32x32크기의 워터마크 크기를 초과하지 않도록 버전1~3의 QR code로 변환한다.

**삽입 알고리즘**   
* 일반적으로 워터마크 삽입 방식은 공간 영역 삽입과 주파수 영역 삽입으로 나뉜다. 프로젝트 목표인 비가시성과 JPEG 압축 강인성 확보를 위해 주파수 영역 삽입 방식으로 구현을 진행한다.   

* **색 변환**: RGB 형식의 이미지 색 채널을 YUV 색 채널로 변환한다. Y는 이미지 휘도, U와 V는 이미지 색차 신호를 나타내는데, JPEG 압축은 사람의 눈이 변화에 가장 무감각한 U, V 채널의 데이터를 제거하는 방식으로 압축을 진행한다. 따라서, 압축 시 제거되지 않는 Y 채널을 대상으로 삽입을 진행하며, 마지막 단계에서 YUV 색 채널을 RGB 형식으로 역 변환하는 과정에서 손실 압축으로 인한 변화를 Y 채널에 최소화 하기 위해 Y 채널을 대상으로 JPEG 압축 방식에 맞추어 삽입을 진행한다.   

* **Discrete wavelet transform(DWT)**: 색 변환을 통해 가져온 이미지의 Y 채널을 대상으로 이진 웨이블릿 변환을 진행하여 공간 영역에서 주파수 영역으로 변환하고 삽입을 진행한다. 이를 통해 2차원 배열의 픽셀 값으로 표현되는 이미지는 2차원 배열의 픽셀 값 변화량으로 표현되는 이미지로 변환된다. 즉, 특정 픽셀의 위치를 기준으로 인접한 픽셀 간의 값의 변화량으로 표현하는 것이다. 또한, DWT의 경우 주파수(픽셀 값 변화량) 분해능만 포함한 일반적인 주파수 변환과는 달리, 주파수 분해능과 변환 대상인 공간 영역(픽셀 값, 해당 위치)에 대한 분해능을 포함하고 있기 때문에 원본 이미지에서 특정 위치의 픽셀 값 변화량을 동시에 표현할 수 있다.       
![DWT](https://user-images.githubusercontent.com/13462458/74600613-57d52a80-50d7-11ea-9aa1-c079f1cd222b.PNG)   
DWT의 결과 LL, LH, HL, HH 4개의 부대역을 얻을 수 있다.      
**이미지에서 주파수의 의미는 픽셀 값 변화량으로 나타낼 수 있으며 저주파의 경우 픽셀 값의 변화량이 적은 것으로 배경에 가까우며, 고주파의 경우 픽셀 값의 변화량이 큰 것으로 경계선에 가깝다.**   
**좌측 상단**은 LL 부대역으로 원본 이미지에 수평, 수직 방향으로 저역 통과 필터를 적용한 것과 같다.   
(정규화 과정을 거치지 않아 높은 값으로 표현된다.)    
**우측 상단**은 HL 부대역으로 원본 이미지에 수직 방향으로 고역 통과 필터를 적용한 것과 같다.    
수직 방향 경계선이 나타난다.   
**좌측 하단**은 LH 부대역으로 원본 이미지에 수평 방향으로 고역 통과 필터를 적용한 것과 같다.    
수평 방향 경계선이 나타난다.   
**우측 하단**은 HH 부대역으로 원본 이미지에 수평, 수직 방향으로 고역 통과 필터를 적용한 것과 같다.    
대각선 방향의 경계선이 나타난다.    
**배경을 나타내는 저주파의 변화는 화질과 사람의 눈에 큰 영향을 준다. 반면에, 경계선을 나타내는 고주파의 변화는 화질과 사람의 눈에 큰 영향을 주지 않으므로 고주파 영역을 포함하는 HL, LH, HH 대역을 선택하여 삽입을 진행한다.**   

* **Discrete cosine transform(DCT)**: 
## Result

웹 데모 버전 링크

제한사항

발전사항
