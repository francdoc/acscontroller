import socket
import struct

"""
DATA FORMAT
+----------------------------------------+-------------+-------------+------------------+
| msgeid (8 bytes)                       | double_1 (8B)| double_2 (8B)| double_3 (8B)    |
+----------------------------------------+-------------+-------------+------------------+
| CONNECT_ACS_SIMULATOR (1)              |      0      |      0      |        0           |
| CONNECT_ACS_REAL_HARDWARE (2)          |      0      |      0      |        0           |
| CLEAR_FAULTS_ACS_STAGE (3)             |      0      |      0      |        0           |
| ENABLE_ACS_STAGE (4)                   |      0      |      0      |        0           |
| COMMUTE_ACS_STAGE (5)                  |      0      |      0      |        0           |
| SET_ACCEL_ACS_STAGE (6)                |      0      |      0      |   acceleration     |
| SET_VEL_ACS_STAGE (7)                  |      0      |  velocity   |        0           |
| MOVE_ACS_STAGE (8)                     |  targetpos  |  targetpos  |    targetpos       |
| HALT_ACS_STAGE (9)                     |      0      |      0      |        0           |
| SET_DECEL_ACS_STAGE (10)               |      0      |      0      |   deceleration     |
| SEND_POS_ACS_STAGE (11)                |    posx     |    posy     |       posa         |
+----------------------------------------+-------------+-------------+------------------+
"""

# Data structure
class ZStageData:
    def __init__(self, dataframe_1, dataframe_2, dataframe_3, dataframe_4):
        self.dataframe1 = dataframe_1
        self.dataframe2 = dataframe_2
        self.dataframe3 = dataframe_3
        self.dataframe4 = dataframe_4
        self.connected = False  # Flag to track the connection status

    def pack(self):
        # Pack data into a binary format (using struct)
        header = struct.pack("!d", self.dataframe1)
        payload = struct.pack("!ddd", self.dataframe2, self.dataframe3, self.dataframe4)
        return header + payload


class ACSClient:
    """Class to connect to ACS server module and control the linear actuator."""

    def __init__(self, server_ip: int, server_port: str):
        self.server_ip = server_ip
        self.server_port = server_port
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def struct(self, dataframe_1, dataframe_2, dataframe_3, dataframe_4):
        res = ZStageData(dataframe_1, dataframe_2, dataframe_3, dataframe_4)
        structed_data = res.pack()
        return structed_data

    def connect_client(self):
        try:
            self.client_socket.connect((self.server_ip, self.server_port))
            self.connected = True  # Connection successful
            return True
        except ConnectionError as ce:
            print(f"Got ConnectionError while connecting to ACS server: {ce}")
            return False
        except Exception as e:
            print(f"Error in connect_client: {e}")
            raise

    def send_data(self, structed_data):
        try:
            self.client_socket.sendall(structed_data)
            res = self.client_socket.recv(1024)
            response_str = res.decode("utf-8")
            return response_str
        except ConnectionResetError as cre:
            raise ConnectionError("ERROR - Connection reset by peer") from cre
        except Exception as e:
            print(f"Error in send_data: {e}")
            raise

    def close_connection(self):
        self.client_socket.close()
        print("Connection closed.")

    def connect_acs_simulator(self):
        dataframe1 = 1
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = 0
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def connect_acs_real_hardware(self):
        dataframe1 = 2
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = 0
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def clear_faults_acs_stage(self):
        dataframe1 = 3
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = 0
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def enable_acs_stage(self):
        dataframe1 = 4
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = 0
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def commute_acs_stage(self):
        dataframe1 = 5
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = 0
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def halt_acs_stage(self):
        dataframe1 = 9
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = 0
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def set_accel_acs_stage(self, accel):
        """mm/s2"""
        dataframe1 = 6
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = accel
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def set_decel_acs_stage(self, decel):
        """mm/s2"""
        dataframe1 = 10
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = decel
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def set_vel_acs_stage(self, vel):
        """mm/s"""
        dataframe1 = 7
        dataframe2 = 0
        dataframe3 = vel
        dataframe4 = 0
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def move_acs_stage(self, targetpos_x, targetpos_y, targetpos_a):
        """mm"""
        dataframe1 = 8
        dataframe2 = targetpos_x
        dataframe3 = targetpos_y
        dataframe4 = targetpos_a
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res).strip("\x00")
        return res_server

    def get_pos(self):
        """(xpos,ypos,apos) [mm,mm,mm]"""
        dataframe1 = 11
        dataframe2 = 0
        dataframe3 = 0
        dataframe4 = 0
        res = self.struct(dataframe1, dataframe2, dataframe3, dataframe4)
        res_server = self.send_data(res)
        pos_string = res_server.strip("\x00")
        pos_values = [float(value.split(":")[1]) for value in pos_string.split()]
        return pos_values
