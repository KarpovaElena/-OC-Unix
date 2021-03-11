import sys
from datetime import datetime
import time
import threading

STORAGE_PATH = '/var/internet_usage/'
STOP_FLAG = False


def get_cur_date():
    return datetime.utcfromtimestamp(int(time.time())).strftime('%Y-%m-%d')


def load_last_time_usage():
    try:
        with open(STORAGE_PATH + get_cur_date(), 'r') as f:
            return float(f.readline())
    except Exception:
        return 0


def write_usage(usage):
    with open(STORAGE_PATH + get_cur_date(), 'w') as f:
        f.write(usage)


def session_writer():
    beg_time = time.time()
    offset = load_last_time_usage()
    print('[T] Thread started: ', beg_time, offset)
    while not STOP_FLAG:
        write_usage(str(time.time() - beg_time + offset))
        time.sleep(1)
    print('[T] Thread finished')


if __name__ == '__main__':
    th = threading.Thread(target=session_writer)
    th.start()
    while True:
        sp_wrds = sys.stdin.readline().split()
        for i in range(len(sp_wrds)):
            if sp_wrds[i] == 'uint32':
                code = int(sp_wrds[i + 1])
                print('code:', code)
                if (code == 40 or code == 20) and th is not None:
                    STOP_FLAG = True
                    try:
                        th.join(5)
                    except Exception:
                        pass
                    th = None
                    STOP_FLAG = False
                    print('[M] Thread stopped')
                if code == 70:
                   th = threading.Thread(target=session_writer)
                   th.start()
