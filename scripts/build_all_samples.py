'''build_all_samples.py

Build all sample applications.'''

import os
import subprocess
from west.commands import WestCommand

class BuildAllSamples(WestCommand):

    def __init__(self):
        super().__init__(
            'build-all-samples',  # self.name
            'Build all sample applications.',  # self.help
            # self.description:
            '''
            Builds each subfolder in samples/ using west build.
            Each application will have its own build directory created under
            the 'build' folder.''')
        
    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name,
            help=self.help,
            description=self.description,
        )
        parser.add_argument('-b', '--board', required=True,
                            help='Board name to build for (e.g. nrf52840dk_nrf52840)')
        parser.add_argument('-p', '--pristine', action='store_true',
                            help='Perform a pristine build for each sample.')
        parser.add_argument('--stop-on-error', action='store_true',
                            help='Stop building other samples if one fails')
        
        return parser
    
    def do_run(self, args, unknown_args):
        # samples_root = os.path.abspath('samples')
        samples_root = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'samples')
        samples_root = os.path.abspath(samples_root)

        if not os.path.isdir(samples_root):
            self.err(f"Samples directory not found: {samples_root}")
            return 1
        
        samples = [
            name for name in os.listdir(samples_root)
            if os.path.exists(os.path.join(samples_root, name, 'CMakeLists.txt'))
        ]
        nb_samples = len(samples)
        if nb_samples == 0:
            self.wrn(f"No samples found in {samples_root}")
            return 0
        
        results = []
        for i, sample in enumerate(samples, start=1):
            path = os.path.join(samples_root, sample)
            build_dir = os.path.join(path, 'build')
            
            self.banner(f"[{i}/{nb_samples}] Building sample: {sample}")

            cmd = ['west', 'build', '-b', args.board, path, '-d', build_dir]
            if args.pristine:
                cmd.append('--pristine')
            result = subprocess.run(cmd)

            if result.returncode == 0:
                self.inf(f"✅ {sample} built successfully\n")
                results.append((sample, True))
            else:
                self.err(f"❌ Failed to build {sample}\n")
                results.append((sample, False))
                if args.stop_on_error:
                    break

        # Summary
        success = sum(1 for _, ok in results if ok)
        self.banner("Summary")
        for name, ok in results:
            status = "OK" if ok else "FAIL"
            self.inf(f" - {name:<30} {status:>5}")

        self.inf(f"{success}/{nb_samples} samples built successfully")

        return 0