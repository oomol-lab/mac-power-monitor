{
  "targets": [
    {
      "target_name": "pm",
      "sources": [
        "src/pm.cpp",
        "src/pm.h"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'conditions': [
        ['OS=="win"',
          {
            'sources': [
              "src/pm_win.cpp"
            ]
          }
        ],
        ['OS=="mac"',
          {
            'sources': [
              "src/pm_mac.cpp"
            ]
          }
        ]
      ]
    }
  ]
}
