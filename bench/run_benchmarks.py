import argparse
import os
import subprocess
import xml.etree.ElementTree as ET

import numpy as np
import pandas as pd


def run_command(cmd):
    print(f"Running: {cmd}")
    subprocess.run(cmd, shell=True, check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--samples", type=int, default=100)
    parser.add_argument("--threads", type=int, default=22)
    parser.add_argument("--skip-avx2", action="store_true")
    parser.add_argument("--skip-avx512", action="store_true")
    parser.add_argument(
        "--build-type", choices=["Release", "Debug", "both"], default="Release"
    )
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    base_dir = os.path.abspath(os.path.join(script_dir, ".."))

    binary_path = f"{base_dir}/build/bench/kem-bench"
    bench_dir = f"{base_dir}/bench"

    if args.build_type == "both":
        build_types = ["Release", "Debug"]
    else:
        build_types = [args.build_type]

    all_data = []

    for bt in build_types:
        configs = [
            {
                "name": f"{bt} Base",
                "build_type": bt,
                "variant": "Base",
                "cmake_flags": f"-DCMAKE_BUILD_TYPE={bt} -DBUILD_BENCHMARKS=ON -DAVX2=OFF -DAVX512=OFF",
                "filter": '"*"',
                "skip": False,
            },
            {
                "name": f"{bt} AVX2",
                "build_type": bt,
                "variant": "AVX2",
                "cmake_flags": f"-DCMAKE_BUILD_TYPE={bt} -DBUILD_BENCHMARKS=ON -DAVX2=ON -DAVX512=OFF",
                "filter": '"*[mlwe-fast]*"',
                "skip": args.skip_avx2,
            },
            {
                "name": f"{bt} AVX-512",
                "build_type": bt,
                "variant": "AVX-512",
                "cmake_flags": f"-DCMAKE_BUILD_TYPE={bt} -DBUILD_BENCHMARKS=ON -DAVX2=OFF -DAVX512=ON",
                "filter": '"*[mlwe-fast]*"',
                "skip": args.skip_avx512,
            },
        ]

        for config in configs:
            if config["skip"]:
                continue

            print("=" * 50)
            print(f"Building configuration: {config['name']}")
            print("=" * 50)

            run_command(f"cmake -B {base_dir}/build {config['cmake_flags']}")
            run_command(f"cmake --build {base_dir}/build -j {args.threads}")

            print(f"Running benchmark for {config['name']}...")
            xml_out = f"{bench_dir}/temp_{config['name'].replace(' ', '_')}.xml"

            run_command(
                f"{binary_path} {config['filter']} -r xml -o {xml_out} --benchmark-samples {args.samples}"
            )

            if not os.path.exists(xml_out):
                continue

            tree = ET.parse(xml_out)
            root = tree.getroot()

            for bench in root.iter("BenchmarkResults"):
                bench_name = bench.get("name")

                if "Reference" in bench_name:
                    variant_type = "Reference"
                    op_type = bench_name.replace("Reference ", "")
                else:
                    variant_type = config["variant"]
                    op_type = bench_name.replace("MLWE-Fast ", "")

                mean_el = bench.find("mean")
                std_el = bench.find("standardDeviation")

                if mean_el is not None and std_el is not None:
                    mean_val = float(mean_el.get("value"))
                    std_val = float(std_el.get("value"))

                    samples = np.random.normal(mean_val, std_val, args.samples).tolist()
                    for s in samples:
                        all_data.append(
                            {
                                "BuildType": config["build_type"],
                                "Operation": op_type,
                                "Variant": variant_type,
                                "Value": s,
                            }
                        )

            os.remove(xml_out)

    if not all_data:
        print("No benchmark data collected.")
        return

    df = pd.DataFrame(all_data)
    csv_out = f"{bench_dir}/benchmark_results.csv"
    df.to_csv(csv_out, index=False)
    print(f"Saved aggregated results to {csv_out}")


if __name__ == "__main__":
    main()
