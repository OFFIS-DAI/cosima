import threading
import pandas as pd
import time
import psutil
import matplotlib.pyplot as plt

from cosima_core.config import ROOT_PATH


class PerformanceTracker(object):
    def __init__(self, interval=0.1):
        self.interval = interval
        self.tracking_df = pd.DataFrame(columns=['time', 'cpu', 'ram'])
        thread = threading.Thread(target=self.run)
        thread.daemon = True
        thread.start()

    def run(self):
        start_time = time.time()
        while True:
            new_row = {'time': round(time.time() - start_time, 2), 'cpu': psutil.cpu_percent(interval=self.interval),
                       'ram': psutil.virtual_memory().percent}
            self.tracking_df = self.tracking_df.append(new_row, ignore_index=True)
            time.sleep(self.interval)

    def save_results(self):
        self.tracking_df.to_csv(ROOT_PATH / 'results' / f'performance_{time.time()}.csv', index=False)
        plt.xlabel('time in seconds')
        plt.ylabel('system-wide CPU utilization as a percentage')
        self.tracking_df['cpu'].plot()
        plt.savefig(ROOT_PATH / 'results' / f'cpu.png')
        plt.close()
        plt.xlabel('time in seconds')
        plt.ylabel('ram usage as a percentage')
        self.tracking_df['ram'].plot()
        plt.savefig(ROOT_PATH / 'results' / f'ram.png')
