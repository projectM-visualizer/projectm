#!/bin/bash

DIR="$( dirname "$_" )"

AUTH=--username thadwooster@gmail.com --password @keychain:AC_PASSWORD


xcrun altool --notarize-app --primary-bundle-id "net.projectm.installer" \
      --asc-provider 'MischaSpiegelmock10100337' \
      $AUTH --file "$DIR/../ProjectM.pkg"

# sleep 60
# xcrun altool --notarization-history 0 $AUTH
# xcrun stapler staple "$DIR/../ProjectM.pkg"
