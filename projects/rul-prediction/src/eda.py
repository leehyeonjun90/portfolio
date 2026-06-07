import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import io

# 세션 상태에서 데이터 가져오기
if 'data_n_lot' in st.session_state:
    data_n_lot = st.session_state.data_n_lot
else:
    st.error("No data found. Please make sure to load and preprocess the data on the main page.")
    st.stop()
    
# Streamlit 앱 제목 및 설명
st.title("EDA")
st.markdown("""
    버튼을 눌러 데이터의 분포를 살펴보세요!
""")

# 데이터프레임을 하나로 결합하여 EDA 수행
data = pd.concat(data_n_lot, ignore_index=True)

# Data Exploration 버튼 추가
if 'explore_data' not in st.session_state:
    st.session_state.explore_data = False

if st.button("Data Exploration"):
    st.session_state.explore_data = True

if st.session_state.explore_data:
    st.write("### Data Exploration")
    st.write("#### Data Head")
    st.write(data.head())

# 데이터 정보 표시
if 'show_data_info' not in st.session_state:
    st.session_state.show_data_info = False

if st.button("데이터 정보 확인"):
    st.session_state.show_data_info = True

if st.session_state.show_data_info:
    st.write("## 데이터 정보")
    buffer = io.StringIO()
    data.info(buf=buffer)
    s = buffer.getvalue()
    st.text(s)

# NULL 값 확인 및 제거 버튼
if 'show_null_info' not in st.session_state:
    st.session_state.show_null_info = False

if st.button("NULL값 확인"):
    st.session_state.show_null_info = True

if st.session_state.show_null_info:
    st.write("## NULL값 확인")
    st.write(data.isnull().sum())

if 'remove_null' not in st.session_state:
    st.session_state.remove_null = False

if st.button("NULL값 제거"):
    st.session_state.remove_null = True

if st.session_state.remove_null:
    data = data.dropna()
    st.success("NULL값이 제거되었습니다.")

# 컬럼 선택을 위한 체크박스
st.write("## 컬럼 선택")
selected_columns = st.multiselect("분석에 사용할 컬럼을 선택하세요:", options=data.columns.tolist(), default=[])

if selected_columns:
    selected_data = data[selected_columns]

    if 'show_distribution' not in st.session_state:
        st.session_state.show_distribution = False

    if st.button("데이터 분포 플롯 보기"):
        st.session_state.show_distribution = True

    if st.session_state.show_distribution:
        # 데이터 분포 플롯
        st.write("## 데이터 분포 플롯")
        fig, axs = plt.subplots(len(selected_columns)//5 + 1, 5, figsize=(30, (len(selected_columns)//5 + 1)*6))
        axs = axs.flatten()
        for i, col in enumerate(selected_columns):
            axs[i].hist(selected_data[col], color=(144/255, 171/255, 221/255), edgecolor='black')
            axs[i].set_title(col)
        for j in range(i + 1, len(axs)):
            fig.delaxes(axs[j])  # 빈 서브플롯 제거
        st.pyplot(fig)

    if 'show_heatmap' not in st.session_state:
        st.session_state.show_heatmap = False

    if st.button("상관관계 히트맵 보기"):
        st.session_state.show_heatmap = True

    if st.session_state.show_heatmap:
        # 상관 행렬 히트맵
        st.write("## 상관관계 히트맵")
        fig, ax = plt.subplots(figsize=(25, 25))
        sns.heatmap(selected_data.corr(), linewidths=0.1, annot=True, fmt='.2f', cmap='Blues', ax=ax)
        st.pyplot(fig)
else:
    st.warning("분석에 사용할 컬럼을 선택하세요.")
