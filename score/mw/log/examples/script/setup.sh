# Set MW_LOG_CONFIG_FILE dynamically
if [ -z "$MW_LOG_CONFIG_FILE" ]; then
    echo "MW_LOG_CONFIG_FILE is not set. Setting it to the default configuration file."
    export MW_LOG_CONFIG_FILE="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")/../etc/logging.json")"
fi