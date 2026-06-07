import pandas as pd
import numpy as np
import streamlit as st
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression

# 세션 상태에서 데이터 가져오기
if 'data_n_lot' in st.session_state:
    data_n_lot = st.session_state.data_n_lot
else:
    st.error("No data found. Please make sure to load and preprocess the data on the main page.")
    st.stop()

@st.cache_data
def load_labeled_data():
    data = pd.read_csv("../data/InjectionMolding_Labeled_Data.csv")
    return data

data_lot_label = load_labeled_data()

# Shot 데이터에 Lot 번호 라벨링
n_Lot_list = []
for i in range(len(data_n_lot)):
    a = i
    Lot_label = data_n_lot[i].copy()
    Lot_label.loc[:,['Lot']] = a
    n_Lot_list.append(Lot_label)

# concat 함수를 통한 데이터 결합
data_lot_label = n_Lot_list[0]
for i in range(len(n_Lot_list)):
    if i ==  0:
        pass
    else:
        data_lot_label = pd.concat([data_lot_label, n_Lot_list[i]])

data_lot_label = data_lot_label.reset_index(drop=True)

st.title("Anomaly Detection")

# 데이터프레임에서 존재하는 Lot 번호 리스트 생성
existing_lot_numbers = data_lot_label['Lot'].unique()

# 사용자 입력 받기
user_input_lots = st.text_input("원하는 Lot 번호를 쉼표로 구분하여 입력하세요", '').strip()  # 공백 제거 추가
if user_input_lots == '':
    st.warning("Lot번호를 지정해주세요!")
    st.stop()

selected_lot_numbers = list(map(int, user_input_lots.split(',')))

# 입력된 Lot 번호 중 데이터프레임에 존재하는 Lot 번호만 필터링
valid_lot_numbers = [lot for lot in selected_lot_numbers if lot in existing_lot_numbers]

# 유효한 Lot 번호에 해당하는 데이터 필터링
filtered_data = data_lot_label[data_lot_label['Lot'].isin(valid_lot_numbers)]

# 결과를 저장할 데이터프레임
residuals_df = pd.DataFrame(index=filtered_data.index)

# 각 레이블에 대해 모델 학습 및 잔차 계산
features = ['Machine_Cycle_Time', 'Cycle_Time', 'Barrel_Temp_Z1', 'Barrel_Temp_Z2', 'Barrel_Temp_Z3', 'Barrel_Temp_Z4', 'Hopper_Temp', 'Injection_Pressure_Real_Time', 'Screw_Position', 'Injection_Peak_Press', 'Max_Injection_Rate', 'Screw_Velocity', 'VP_Time', 'VP_Position', 'VP_Press', 'Plasticizing_Time', 'Plasticizing_Start_Position', 'Plasticizing_End_Position', 'Plasticizing_RPM', 'Cooling_Time', 'Back_Flow', 'Decompression_Time']

# 전체 데이터를 사용하여 모델 학습
for label in features:
    X = data_lot_label.drop(columns=[label, 'Lot'])
    y = data_lot_label[label]
    model = LinearRegression()
    model.fit(X, y)

    # 필터링된 데이터에 대해 예측 및 잔차 계산
    X_filtered = filtered_data.drop(columns=[label, 'Lot'])
    y_filtered = filtered_data[label]
    y_pred = model.predict(X_filtered)
    residuals = y_filtered - y_pred

    residuals_df[label] = residuals

# 누적 잔차 계산
cumulative_residuals = residuals_df.cumsum()

# Hotelling's T-squared 통계량 계산
mean_vector = cumulative_residuals.mean(axis=0)
cov_matrix = np.cov(cumulative_residuals.T)

# 공분산 행렬의 역행렬 계산
inv_cov_matrix = np.linalg.inv(cov_matrix)

# Hotelling's T-squared 계산 함수
def hotelling_t2(row, mean_vector, inv_cov_matrix):
    diff = row - mean_vector
    t_squared = np.dot(np.dot(diff, inv_cov_matrix), diff.T)
    return t_squared

# 각 행에 대해 Hotelling's T-squared 계산
cumulative_residuals['T_squared'] = cumulative_residuals.apply(hotelling_t2, axis=1, args=(mean_vector, inv_cov_matrix))

st.write("## Hotelling's T-squared Results")
st.write(cumulative_residuals['T_squared'])

# Lot 번호별로 T-squared 결과값을 구분하여 시각화
unique_lots = filtered_data['Lot'].unique()

# Lot 번호가 하나인 경우
if len(unique_lots) == 1:
    lot = unique_lots[0]
    lot_indices = filtered_data[filtered_data['Lot'] == lot].index
    fig, ax = plt.subplots(figsize=(15, 8))
    ax.plot(cumulative_residuals.loc[lot_indices].index, cumulative_residuals.loc[lot_indices]['T_squared'], label=f'Lot {lot}')
    ax.set_xlabel('Index')
    ax.set_ylabel("Hotelling's T-squared")
    ax.set_title(f"Hotelling's T-squared over Time for Lot {lot}")
    ax.legend()
    st.pyplot(fig)

# Lot 번호가 여러 개인 경우
else:
    for lot in unique_lots:
        lot_indices = filtered_data[filtered_data['Lot'] == lot].index
        fig, ax = plt.subplots(figsize=(15, 8))
        ax.plot(cumulative_residuals.loc[lot_indices].index, cumulative_residuals.loc[lot_indices]['T_squared'], label=f'Lot {lot}')
        ax.set_xlabel('Index')
        ax.set_ylabel("Hotelling's T-squared")
        ax.set_title(f"Hotelling's T-squared over Time for Lot {lot}")
        ax.legend()
        st.pyplot(fig)

# Bootstrap 기반 신뢰 구간 계산 및 RUL 커브 시각화
for lot in unique_lots:
    lot_indices = filtered_data[filtered_data['Lot'] == lot].index
    lot_data = cumulative_residuals.loc[lot_indices]

    # Bootstrap 기반 신뢰 구간 계산
    n_iterations = 1000
    t_squared_bootstrap = np.zeros((n_iterations, len(lot_data)))

    for i in range(n_iterations):
        sample_indices = np.random.choice(lot_data.index, size=len(lot_data), replace=True)
        sample = lot_data.loc[sample_indices]
        sample_mean_vector = sample.mean(axis=0)
        sample_cov_matrix = np.cov(sample.T)
        sample_inv_cov_matrix = np.linalg.inv(sample_cov_matrix)

        t_squared_sample = sample.apply(hotelling_t2, axis=1, args=(sample_mean_vector, sample_inv_cov_matrix))
        t_squared_bootstrap[i, :] = t_squared_sample

    # 신뢰 구간 계산
    confidence_level = 0.99
    lower_bound = np.percentile(t_squared_bootstrap, (1 - confidence_level) / 2 * 100, axis=0)
    upper_bound = np.percentile(t_squared_bootstrap, (1 + confidence_level) / 2 * 100, axis=0)

    # RUL 커브 시각화
    fig, ax = plt.subplots(figsize=(15, 8))
    ax.plot(lot_data.index, lot_data['T_squared'], label=f'Lot {lot}')
    ax.fill_between(lot_data.index, lower_bound, upper_bound, color='b', alpha=0.2, label='Confidence Interval')
    ax.set_xlabel('Index')
    ax.set_ylabel('Hotelling\'s T-squared')
    ax.set_title(f'Hotelling\'s T-squared with Confidence Interval for {lot}')

    # 이상 감지 시각화
    anomalies = (lot_data['T_squared'] > upper_bound) | (lot_data['T_squared'] < lower_bound)
    ax.scatter(lot_data.index[anomalies], lot_data['T_squared'][anomalies], color='r', label='Anomalies')

    ax.legend()
    st.pyplot(fig)
