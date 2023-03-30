from pysimmods.generator.chplpgsystemsim.chplpg_system import CHPLPGSystem
from pysimmods.generator.chplpgsystemsim.presets import chp_preset
from numpy.random import normal
import numpy as np
import pandas as pd
from datetime import datetime, timedelta, timezone


def generate_schedules_from_pysimmods(n_schedules, steps):
    step_size = 900  # 15 minutes

    chp_types = [
        {'name': 'CHP 1.1', 'p_kw_max': 7, 'start_state': 7},
        {'name': 'CHP 2.1', 'p_kw_max': 7, 'start_state': 6},
        {'name': 'CHP 3.1', 'p_kw_max': 7, 'start_state': 5},
        {'name': 'CHP 4.1', 'p_kw_max': 7, 'start_state': 7},
        {'name': 'CHP 5.1', 'p_kw_max': 7, 'start_state': 6},
        {'name': 'CHP 6.1', 'p_kw_max': 7, 'start_state': 5},

        {'name': 'CHP 7.2', 'p_kw_max': 14, 'start_state': 10},
        {'name': 'CHP 8.2', 'p_kw_max': 14, 'start_state': 11},
        {'name': 'CHP 9.2', 'p_kw_max': 14, 'start_state': 12},
        {'name': 'CHP 10.2', 'p_kw_max': 14, 'start_state': 13},
        {'name': 'CHP 11.2', 'p_kw_max': 14, 'start_state': 14},

        {'name': 'CHP 12.3', 'p_kw_max': 200, 'start_state': 180},
        {'name': 'CHP 13.3', 'p_kw_max': 200, 'start_state': 190},
        {'name': 'CHP 14.3', 'p_kw_max': 200, 'start_state': 200},
        {'name': 'CHP 15.3', 'p_kw_max': 200, 'start_state': 180},
        {'name': 'CHP 16.3', 'p_kw_max': 200, 'start_state': 190},

        {'name': 'CHP 17.4', 'p_kw_max': 400, 'start_state': 350},
        {'name': 'CHP 18.4', 'p_kw_max': 400, 'start_state': 360},
        {'name': 'CHP 19.4', 'p_kw_max': 400, 'start_state': 370},
        {'name': 'CHP 20.4', 'p_kw_max': 400, 'start_state': 380},
    ]
    start_temperature = 20 + normal(loc=0, scale=3, size=1)[0]
    temperatures = dict()
    for i in range(n_schedules):
        temperatures[i] = list()
        temperature = start_temperature
        for step in range(steps):
            temperatures[i].append(temperature)
            temperature += normal(loc=0, scale=1, size=1)[0]
    now_dt = datetime(2021, 6, 8, 0, 0, 0, tzinfo=timezone.utc)
    for current_chp in chp_types:
        schedule_dict = dict()
        name = current_chp['name']
        state = current_chp['start_state']
        p_kw_max = current_chp['p_kw_max']
        chpsys = CHPLPGSystem(*chp_preset(p_kw=p_kw_max))
        chpsys.inputs.p_set_kw = state

        for n_schedule in range(n_schedules):
            p_kws = np.zeros(steps)
            for step in range(steps):
                chpsys.set_step_size(step_size)
                chpsys.set_now_dt(now_dt)
                chpsys.inputs.day_avg_t_air_deg_celsius = temperatures[n_schedule][step]
                chpsys.step()
                p_kws[step] = chpsys.get_p_kw()
                now_dt += timedelta(seconds=step_size)
            schedule_dict[n_schedule] = [abs(i + round(normal(loc=0, scale=1, size=steps)[0])) for i in p_kws]

        schedule_df = pd.DataFrame.from_dict(schedule_dict)

        name = f'../../data/{name}_state_{state}_max_p_kw_{p_kw_max}'
        schedule_df.to_csv(f'{name}.csv', index=False)


generate_schedules_from_pysimmods(n_schedules=20, steps=4)
