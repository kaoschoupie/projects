input_folder = r"insert complete path here"
output_folder = r"insert complete path here"

import os
import subprocess
from pathlib import Path

def convert_wav_to_flac(input_dir,output_dir):
    input_dir = Path(input_dir)
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    wav_files = list(input_dir.rglob("*.wav"))

    if not wav_files:
        print("No WAV files found.")
        return    

    for wav_path in wav_files:
        relative_path = wav_path.relative_to(input_dir)
        flac_path = output_dir / relative_path.with_suffix(".flac")
        flac_path.parent.mkdir(parents=True, exist_ok=True)

        if flac_path.exists():
            print(f"Skipping (already exists): {flac_path}")
            continue

        print(f"Converting: {wav_path} → {flac_path}")

        cmd = [
            "ffmpeg",
            "-i", str(wav_path),
            "-map_metadata", "0",      # copy metadata
            "-codec:a", "flac",
            "-compression_level", "5",
            "-y",                      # overwrite output
            str(flac_path)
        ]

        subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print("Conversion complete.")

def convert_flac_to_mp3(input_dir, output_dir, bitrate="320k"):
    input_dir = Path(input_dir)
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    flac_files = list(input_dir.rglob("*.flac"))

    if not flac_files:
        print("No FLAC files found.")
        return

    for flac_path in flac_files:
        relative_path = flac_path.relative_to(input_dir)
        mp3_path = output_dir / relative_path.with_suffix(".mp3")
        mp3_path.parent.mkdir(parents=True, exist_ok=True)

        if mp3_path.exists():
            print(f"Skipping (already exists): {mp3_path}")
            continue

        print(f"Converting: {flac_path} → {mp3_path}")

        cmd = [
            "ffmpeg",
            "-i", str(flac_path),
            "-map_metadata", "0",      # copy metadata
            "-codec:a", "libmp3lame",
            "-b:a", bitrate,
            "-compression_level", "0",
            "-y",                      # overwrite output
            str(mp3_path)
        ]

        subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    print("Conversion complete.")

if __name__ == "__main__":
    #convert_flac_to_mp3(input_folder, output_folder)
    convert_wav_to_flac(input_folder,output_folder)