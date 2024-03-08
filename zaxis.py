import os
import sys
import time  # Import the time module for sleep

file_dir = os.path.dirname(__file__)
sys.path.append(file_dir)

from acsclientcontroller import *

flag_axis = False

MAX_VEL = 2.0  # mm/s (Adjust as needed)
MAX_ACCEL = 2.0  # mm/s^2 (Adjust as needed)
MAX_DECEL = 2.0  # mm/s^2 (Adjust as needed)


class Axis:
    """Class to control a z ACS controller."""

    def __init__(self, mechres=0):
        if not flag_axis:
            self.b = ACSClient("127.0.0.1", 4000)
            res = self.b.connect_client()
            if not res:
                raise ValueError("ERROR: Could not connect to ACSClient")
            res = self.b.connect_acs_simulator()
            if res != "OK":
                raise ValueError("ERROR: Could not connect to ACS simulator")
        else:
            self.b = ACSClient("100.0.0.10", 9999)
            res = self.b.connect_client()
            if res != "OK":
                raise ValueError("ERROR: Could not connect to ACSClient")
            res = self.b.connect_acs_real_hardware()
            if res != "OK":
                raise ValueError("ERROR: Could not connect to real hardware")

    def init(self):
        if not flag_axis:
            res = self.b.connect_acs_simulator()
            if res != "OK":
                raise ValueError("ERROR: Could not connect to ACS simulator")
        else:
            res = self.b.connect_acs_real_hardware()
            if res != "OK":
                raise ValueError("ERROR: Could not connect to real hardware")

        res = self.b.clear_faults_acs_stage()
        if res != "OK":
            raise ValueError("ERROR: Could not clear faults in ACS stage")
        res = self.b.enable_acs_stage()
        if res != "OK":
            raise ValueError("ERROR: Could not enable ACS stage")
        res = self.b.commute_acs_stage()
        if res != "OK":
            raise ValueError("ERROR: Could not commute ACS stage")

    def recover(self):
        if not flag_axis:
            res = self.b.connect_acs_simulator()
            if res != "OK":
                raise ValueError("ERROR: Could not connect to ACS simulator")
        else:
            res = self.b.connect_acs_real_hardware()
            if res != "OK":
                raise ValueError("ERROR: Could not connect to real hardware")

        res = self.b.clear_faults_acs_stage()
        if res != "OK":
            raise ValueError("ERROR: Could not clear faults in ACS stage")
        res = self.b.enable_acs_stage()
        if res != "OK":
            raise ValueError("ERROR: Could not enable ACS stage")

    def command_movr_mm(
        self, speed: float, accel: float, decel: float, distance: float
    ):
        """Relative stage movement."""
        try:
            if speed > MAX_VEL:
                raise ValueError(
                    f"Speed value must be less than or equal to {MAX_VEL} mm/s"
                )
            if accel > MAX_ACCEL:
                raise ValueError(
                    f"Acceleration value must be less than or equal to {MAX_ACCEL} mm/s^2"
                )
            if decel > MAX_DECEL:
                raise ValueError(
                    f"Deceleration value must be less than or equal to {MAX_DECEL} mm/s^2"
                )

            res = self.b.set_vel_acs_stage(speed)
            if res != "OK":
                return f"ERROR - {res}"
            res = self.b.set_accel_acs_stage(accel)
            if res != "OK":
                return f"ERROR - {res}"
            res = self.b.set_decel_acs_stage(decel)
            if res != "OK":
                return f"ERROR - {res}"
            res = self.b.move_acs_stage(distance, distance, distance)
            if res != "OK":
                return f"ERROR - {res}"
            return "OK"
        except ConnectionError as ce:
            return f"ERROR - ConnectionError: {ce}"
        except ValueError as ve:
            return f"ERROR - {ve}"

    def command_movr_mm_(
        self,
        speed: float,
        accel: float,
        decel: float,
        distance_x: float,
        distance_y: float,
        distance_a,
    ):
        """Relative stage movement."""
        try:
            if speed > MAX_VEL:
                raise ValueError(
                    f"Speed value must be less than or equal to {MAX_VEL} mm/s"
                )
            if accel > MAX_ACCEL:
                raise ValueError(
                    f"Acceleration value must be less than or equal to {MAX_ACCEL} mm/s^2"
                )
            if decel > MAX_DECEL:
                raise ValueError(
                    f"Deceleration value must be less than or equal to {MAX_DECEL} mm/s^2"
                )

            res = self.b.set_vel_acs_stage(speed)
            if res != "OK":
                return f"ERROR - {res}"
            res = self.b.set_accel_acs_stage(accel)
            if res != "OK":
                return f"ERROR - {res}"
            res = self.b.set_decel_acs_stage(decel)
            if res != "OK":
                return f"ERROR - {res}"
            res = self.b.move_acs_stage(distance_x, distance_y, distance_a)
            if res != "OK":
                return f"ERROR - {res}"
            return "OK"
        except ConnectionError as ce:
            return f"ERROR - ConnectionError: {ce}"
        except ValueError as ve:
            return f"ERROR - {ve}"

    def update(self):
        try:
            res = self.b.get_pos()
            if (
                (isinstance(res, tuple) or isinstance(res, list))
                and len(res) == 3
                and all(isinstance(val, float) for val in res)
            ):
                return res
            else:
                return "ERROR - Unexpected position data"
        except ConnectionError as ce:
            return f"ERROR - ConnectionError: {ce}"


if __name__ == "__main__":
    """Test code for the API"""
    z = Axis()

    # Move forward
    result = z.command_movr_mm(speed=1.5, accel=1.5, decel=1.5, distance=10.0)
    if result == "OK":
        print(z.update())
    else:
        print(result)

    # Move backward
    result = z.command_movr_mm(speed=1.5, accel=1.5, decel=1.5, distance=-10.0)
    if result == "OK":
        print(z.update())
    else:
        print(result)
