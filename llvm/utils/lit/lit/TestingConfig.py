import os
import sys


class TestingConfig:
    """"
    TestingConfig - Information on the tests inside a suite.
    """

    @staticmethod
    def fromdefaults(litConfig):
        """
        fromdefaults(litConfig) -> TestingConfig

        Create a TestingConfig object with default values.
        """
        # Set the environment based on the command line arguments.
        environment = {
            'PATH' : os.pathsep.join(litConfig.path +
                                     [os.environ.get('PATH','')]),
            'LLVM_DISABLE_CRASH_REPORT' : '1',
            }

        pass_vars = ['LIBRARY_PATH', 'LD_LIBRARY_PATH', 'SYSTEMROOT', 'TERM',
                     'LD_PRELOAD', 'ASAN_OPTIONS', 'UBSAN_OPTIONS',
                     'LSAN_OPTIONS', 'ADB', 'ANDROID_SERIAL',
                     'SANITIZER_IGNORE_CVE_2016_2143', 'TMPDIR', 'TMP', 'TEMP',
                     'TEMPDIR', 'AVRLIT_BOARD', 'AVRLIT_PORT']
        for var in pass_vars:
            val = os.environ.get(var, '')
            # Check for empty string as some variables such as LD_PRELOAD cannot be empty
            # ('') for OS's such as OpenBSD.
            if val:
                environment[var] = val

        if sys.platform == 'win32':
            environment.update({
                    'INCLUDE' : os.environ.get('INCLUDE',''),
                    'PATHEXT' : os.environ.get('PATHEXT',''),
                    'PYTHONUNBUFFERED' : '1',
                    'TEMP' : os.environ.get('TEMP',''),
                    'TMP' : os.environ.get('TMP',''),
                    })

        # Set the default available features based on the LitConfig.
        available_features = []
        if litConfig.useValgrind:
            available_features.append('valgrind')
            if litConfig.valgrindLeakCheck:
                available_features.append('vg_leak')

        return TestingConfig(None,
                             name = '<unnamed>',
                             suffixes = set(),
                             test_format = None,
                             environment = environment,
                             substitutions = [],
                             unsupported = False,
                             test_exec_root = None,
                             test_source_root = None,
                             excludes = [],
                             available_features = available_features,
                             pipefail = True)

    def load_from_path(self, path, litConfig):
        """
        load_from_path(path, litConfig)

        Load the configuration module at the provided path into the given config
        object.
        """

        # Load the config script data.
        data = None
        f = open(path)
        try:
            data = f.read()
        except:
            litConfig.fatal('unable to load config file: %r' % (path,))
        f.close()

        # Execute the config script to initialize the object.
        cfg_globals = dict(globals())
        cfg_globals['config'] = self
        cfg_globals['lit_config'] = litConfig
        cfg_globals['__file__'] = path
        try:
            exec(compile(data, path, 'exec'), cfg_globals, None)
            if litConfig.debug:
                litConfig.note('... loaded config %r' % path)
        except SystemExit:
            e = sys.exc_info()[1]
            # We allow normal system exit inside a config file to just
            # return control without error.
            if e.args:
                raise
        except:
            import traceback
            litConfig.fatal(
                'unable to parse config file %r, traceback: %s' % (
                    path, traceback.format_exc()))

        self.finish(litConfig)

    def __init__(self, parent, name, suffixes, test_format,
                 environment, substitutions, unsupported,
                 test_exec_root, test_source_root, excludes,
                 available_features, pipefail, limit_to_features = [],
                 is_early = False, parallelism_group = ""):
        self.parent = parent
        self.name = str(name)
        self.suffixes = set(suffixes)
        self.test_format = test_format
        self.environment = dict(environment)
        self.substitutions = list(substitutions)
        self.unsupported = unsupported
        self.test_exec_root = test_exec_root
        self.test_source_root = test_source_root
        self.excludes = set(excludes)
        self.available_features = set(available_features)
        self.pipefail = pipefail
        # This list is used by TestRunner.py to restrict running only tests that
        # require one of the features in this list if this list is non-empty.
        # Configurations can set this list to restrict the set of tests to run.
        self.limit_to_features = set(limit_to_features)
        # Whether the suite should be tested early in a given run.
        self.is_early = bool(is_early)
        self.parallelism_group = parallelism_group

    def finish(self, litConfig):
        """finish() - Finish this config object, after loading is complete."""

        self.name = str(self.name)
        self.suffixes = set(self.suffixes)
        self.environment = dict(self.environment)
        self.substitutions = list(self.substitutions)
        if self.test_exec_root is not None:
            # FIXME: This should really only be suite in test suite config
            # files. Should we distinguish them?
            self.test_exec_root = str(self.test_exec_root)
        if self.test_source_root is not None:
            # FIXME: This should really only be suite in test suite config
            # files. Should we distinguish them?
            self.test_source_root = str(self.test_source_root)
        self.excludes = set(self.excludes)

    @property
    def root(self):
        """root attribute - The root configuration for the test suite."""
        if self.parent is None:
            return self
        else:
            return self.parent.root

