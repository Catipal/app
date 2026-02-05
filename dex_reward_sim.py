#!/usr/bin/env python3
"""
dex_reward_sim.py
Deterministic simulation of hybrid-DEX reward model (per your spec).
Outputs:
 - CSVs: scenario_<i>_nodes.csv, scenario_<i>_lps.csv
 - PNGs: scenario_<i>_hist_node.png, scenario_<i>_lorenz_node.png
 - Printed summary tables to stdout

Dependencies: numpy, pandas, matplotlib, scipy
Install: pip install numpy pandas matplotlib scipy
Run: python dex_reward_sim.py
"""
import os
import math
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# RNG seed for reproducibility
SEED = 42
np.random.seed(SEED)

# Simulation constants
MATCHES = 100_000
r0 = 0.001
beta = 0.2
V0 = 1000.0
EPS = 1e-6

OUT_DIR = "sim_outputs"
os.makedirs(OUT_DIR, exist_ok=True)

def fee_rate(V):
    return r0 * (1.0 + beta * math.log10(1.0 + V / V0))

def fee_pool(V):
    fr = fee_rate(V)
    return V * fr

def gini(array):
    x = np.array(array, dtype=float)
    if x.size == 0:
        return 0.0
    # Negative/zero safe
    x = np.clip(x, 0.0, None)
    if x.sum() == 0:
        return 0.0
    x = np.sort(x)
    n = x.size
    index = np.arange(1, n+1)
    return (2.0 * np.sum(index * x) / (n * x.sum()) - (n + 1) / n)

def lorenz_curve(x):
    x = np.array(x, dtype=float)
    x = np.clip(x, 0.0, None)
    total = x.sum()
    if total == 0:
        return np.linspace(0, 1, len(x)+1), np.linspace(0, 1, len(x)+1)
    xs = np.sort(x)
    cum = np.cumsum(xs) / total
    cum = np.concatenate(([0.0], cum))
    p = np.linspace(0.0, 1.0, len(cum))
    return p, cum

def summary_stats(rewards):
    rewards = np.array(rewards, dtype=float)
    total = rewards.sum()
    mean = rewards.mean()
    std = rewards.std()
    g = gini(rewards)
    never = np.sum(rewards == 0) / rewards.size
    N = rewards.size
    topk = max(1, int(math.ceil(0.10 * N)))
    top_share = np.sum(np.sort(rewards)[-topk:]) / total if total > 0 else 0.0
    return {
        "total": total,
        "mean": mean,
        "std": std,
        "gini": g,
        "fraction_never_won": never,
        "top10%_share": top_share
    }

def run_scenario(scenario_id, nodes_uptime, lp_balances, V_sampler):
    N = len(nodes_uptime)
    M = len(lp_balances)
    nodes_uptime = np.array(nodes_uptime, dtype=float)
    lp_balances = np.array(lp_balances, dtype=float)
    if lp_balances.sum() <= 0:
        raise ValueError("LP balances must sum > 0")
    # state
    last_reward_at = np.zeros(N, dtype=int)  # per spec initial 0
    node_rewards = np.zeros(N, dtype=float)
    node_wins_count = np.zeros(N, dtype=int)
    node_win_times = [[] for _ in range(N)]
    lp_rewards = np.zeros(M, dtype=float)
    # For speed precompute per-match when V fixed
    fixed_V = None
    if callable(V_sampler):
        pass
    else:
        fixed_V = float(V_sampler)
        F = fee_pool(fixed_V)
        node_pool = 0.5 * F
        lp_pool = node_pool

    for t in range(1, MATCHES + 1):
        # determine V for this match
        if fixed_V is None:
            V = V_sampler()
            F = fee_pool(V)
            node_pool = 0.5 * F
            lp_pool = node_pool

        # Select winner per rules
        max_uptime = nodes_uptime.max()
        candidates = np.where(np.abs(nodes_uptime - max_uptime) <= EPS)[0]
        if candidates.size == 1:
            winner = candidates[0]
        else:
            time_wait = t - last_reward_at[candidates]
            # choose candidate with max time_wait; if multiple, pick first (deterministic)
            idx = np.argmax(time_wait)
            winner = candidates[idx]
        # award
        last_reward_at[winner] = t
        node_rewards[winner] += node_pool
        node_wins_count[winner] += 1
        node_win_times[winner].append(t)
        # LP distribution pro-rata
        lp_share = lp_pool * (lp_balances / lp_balances.sum())
        lp_rewards += lp_share

    # metrics
    node_stats = summary_stats(node_rewards)
    lp_stats = summary_stats(lp_rewards)

    # additional: average time between rewards for nodes that won
    intervals = []
    for wins in node_win_times:
        if len(wins) >= 2:
            diffs = np.diff(wins)
            intervals.append(np.mean(diffs))
    avg_time_between_rewards = float(np.mean(intervals)) if intervals else float('nan')

    # create outputs
    nodes_df = pd.DataFrame({
        "node_id": np.arange(N),
        "uptime": nodes_uptime,
        "total_reward": node_rewards,
        "wins": node_wins_count,
        "last_reward_at": last_reward_at
    })
    nodes_df.to_csv(os.path.join(OUT_DIR, f"scenario_{scenario_id}_nodes.csv"), index=False)

    lps_df = pd.DataFrame({
        "lp_id": np.arange(M),
        "balance": lp_balances,
        "total_reward": lp_rewards
    })
    lps_df.to_csv(os.path.join(OUT_DIR, f"scenario_{scenario_id}_lps.csv"), index=False)

    # plots: node histogram
    plt.figure(figsize=(6,4))
    plt.hist(node_rewards, bins=50, log=True, color='C0', edgecolor='black')
    plt.title(f"Scenario {scenario_id} Node Reward Histogram (log scale)")
    plt.xlabel("Total reward")
    plt.ylabel("Count (log)")
    plt.tight_layout()
    plt.savefig(os.path.join(OUT_DIR, f"scenario_{scenario_id}_hist_node.png"))
    plt.close()

    # Lorenz plot
    p, cum = lorenz_curve(node_rewards)
    plt.figure(figsize=(6,4))
    plt.plot(p, cum, label='Lorenz curve')
    plt.plot([0,1], [0,1], '--', color='gray', label='Equality')
    plt.title(f"Scenario {scenario_id} Node Lorenz Curve (Gini={node_stats['gini']:.3f})")
    plt.xlabel("Cumulative share of nodes")
    plt.ylabel("Cumulative share of rewards")
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(OUT_DIR, f"scenario_{scenario_id}_lorenz_node.png"))
    plt.close()

    # print summary
    print(f"\n--- Scenario {scenario_id} Summary ---")
    print(f"N nodes = {N}, M LPs = {M}, MATCHES = {MATCHES}")
    print(f"Total fees generated (approx): {node_stats['total'] + lp_stats['total']:.2f}")
    print("Node metrics:")
    for k,v in node_stats.items():
        print(f"  {k}: {v}")
    print("LP metrics:")
    for k,v in lp_stats.items():
        print(f"  {k}: {v}")
    print(f"Avg time between rewards (winners): {avg_time_between_rewards:.2f} matches")
    # top 10% share
    print(f"Top-10% node share: {node_stats['top10%_share']:.3f}")
    return {
        "nodes_df": nodes_df,
        "lps_df": lps_df,
        "node_stats": node_stats,
        "lp_stats": lp_stats
    }

def scenario_fixed_V(N, uptime_values, lp_count, lp_balance_sampler, V_fixed=10000.0, scenario_id=1):
    nodes_uptime = np.array(uptime_values)
    lp_balances = lp_balance_sampler(lp_count)
    return run_scenario(scenario_id, nodes_uptime, lp_balances, V_fixed)

def scenario_random_V(N, uptime_values, lp_count, lp_balance_sampler, scenario_id=5):
    nodes_uptime = np.array(uptime_values)
    lp_balances = lp_balance_sampler(lp_count)
    # lognormal median 1000 => mu = ln(1000)
    mu = math.log(1000.0)
    sigma = 1.2
    rng = np.random.default_rng(SEED + scenario_id)
    def sample_V():
        return float(rng.lognormal(mean=mu, sigma=sigma))
    return run_scenario(scenario_id, nodes_uptime, lp_balances, sample_V)

def lp_equal_balances(count, val=100.0):
    return np.full(count, val, dtype=float)

def lp_exponential(count, mean=100.0):
    rng = np.random.default_rng(SEED + count)
    return rng.exponential(scale=mean, size=count)

# Run scenarios
if __name__ == "__main__":
    # Scenario 1
    print("Running Scenario 1 ...")
    sc1 = scenario_fixed_V(
        N=5,
        uptime_values=[0.99]*5,
        lp_count=10,
        lp_balance_sampler=lambda c: lp_equal_balances(c, 100.0),
        V_fixed=10000.0,
        scenario_id=1
    )

    # Scenario 2
    print("Running Scenario 2 ...")
    uptimes_s2 = np.array([0.99]*10 + [0.1]*40)
    sc2 = scenario_fixed_V(
        N=50,
        uptime_values=uptimes_s2,
        lp_count=50,
        lp_balance_sampler=lambda c: lp_exponential(c, mean=100.0),
        V_fixed=10000.0,
        scenario_id=2
    )

    # Scenario 3
    print("Running Scenario 3 ...")
    uptimes_s3 = np.array([0.99]*50 + [0.01]*450)
    sc3 = scenario_fixed_V(
        N=500,
        uptime_values=uptimes_s3,
        lp_count=200,
        lp_balance_sampler=lambda c: lp_exponential(c, mean=100.0),
        V_fixed=10000.0,
        scenario_id=3
    )

    # Scenario 4 (Sybil)
    print("Running Scenario 4 ...")
    uptimes_s4 = np.array([0.9]*100 + [0.01]*400)
    sc4 = scenario_fixed_V(
        N=500,
        uptime_values=uptimes_s4,
        lp_count=200,
        lp_balance_sampler=lambda c: lp_exponential(c, mean=100.0),
        V_fixed=10000.0,
        scenario_id=4
    )

    # Scenario 5 (varied trade volume lognormal)
    print("Running Scenario 5 ...")
    uptimes_s5 = np.array([0.8]*25)
    sc5 = scenario_random_V(
        N=25,
        uptime_values=uptimes_s5,
        lp_count=25,
        lp_balance_sampler=lambda c: lp_equal_balances(c, 100.0),
        scenario_id=5
    )

    print("\nSimulation completed. Outputs written to directory:", OUT_DIR)
    print("CSV per-scenario files: scenario_<i>_nodes.csv, scenario_<i>_lps.csv")
    print("Plots: scenario_<i>_hist_node.png, scenario_<i>_lorenz_node.png")