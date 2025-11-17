// AirLab - Translations
const translations = {
    en: {
        // Header
        title: "AirLab - Pneumatic Test Bench",
        debugMode: "Debug Mode - AirLab",
        signalsTitle: "Digital Signals - AirLab",
        
        // Buttons
        calibrate: "Calibrate",
        reset: "Reset",
        unit: "Unit",
        shutdown: "Shutdown Raspberry Pi",
        reboot: "Reboot Raspberry Pi",
        
        // Navigation
        backToMain: "Back to Main View",
        debugPage: "Debug Page",
        signalsPage: "Digital Signals",
        
        // Channel labels
        vacuum1: "Vacuum 1",
        vacuum2: "Vacuum 2",
        vacuum3: "Vacuum 3",
        vacuum4: "Vacuum 4",
        
        // Digital inputs
        input1: "Input 1",
        input2: "Input 2",
        input3: "Input 3",
        input4: "Input 4",
        input5: "Input 5",
        input6: "Input 6",
        input7: "Input 7",
        input8: "Input 8",
        
        // Status
        statusOk: "OK",
        statusError: "ERROR",
        statusWarning: "WARNING",
        statusOn: "ON",
        statusOff: "OFF",
        
        // Debug info
        rawValue: "Raw Value:",
        offset: "Offset:",
        instantPressure: "Instant Pressure:",
        samplesInAvg: "Samples in Avg:",
        
        // Info messages
        debugInfo: "Debug information updates at 2Hz",
        lastUpdate: "Last update:",
        digitalInputsInfo: "Digital Inputs Status - Updates at 2Hz",
        signalsUpdateInfo: "Digital signals update automatically at 2Hz",
        
        // Calibration instructions
        calibrationTitle: "Calibration Instructions",
        calibrationText: "To calibrate: Expose all suction cups to atmospheric pressure (no vacuum), then click \"Calibrate All Channels\" on the main page to zero the readings.",
        movingAverage: "Moving average filter: 10 samples for stable readings",
        systemStatus: "System status: Make sure",
        isRunning: "is running!",
        
        // Confirmation dialogs
        shutdownConfirm: "Are you sure you want to shutdown the Raspberry Pi?\n\nThis will turn off the system completely.",
        rebootConfirm: "Are you sure you want to reboot the Raspberry Pi?\n\nThe system will restart and the connection will be temporarily lost.",
        resetConfirm: "Reset all calibration offsets to zero?",
        
        // Success/Error messages
        calibrationSuccess: "All channels calibrated to 0 mbar",
        calibrationFailed: "Calibration failed:",
        resetSuccess: "Calibration reset to factory defaults",
        resetFailed: "Reset failed:",
        shutdownSuccess: "Raspberry Pi is shutting down...",
        shutdownFailed: "Shutdown failed:",
        rebootSuccess: "Raspberry Pi is rebooting...",
        rebootFailed: "Reboot failed:",
        connectionError: "Connection error:",
        readError: "Failed to read data from EtherCAT master"
    },
    pt: {
        // Header
        title: "AirLab - Bancada de Testes PneumÃ¡ticos",
        debugMode: "Modo Debug - AirLab",
        
        // Buttons
        calibrate: "Calibrar",
        reset: "Resetar",
        unit: "Unidade",
        shutdown: "Desligar Raspberry Pi",
        reboot: "Reiniciar Raspberry Pi",
        
        // Navigation
        backToMain: "Voltar Ã  Vista Principal",
        debugPage: "PÃ¡gina de Debug",
        
        // Channel labels
        vacuum1: "VÃ¡cuo 1",
        vacuum2: "VÃ¡cuo 2",
        vacuum3: "VÃ¡cuo 3",
        vacuum4: "VÃ¡cuo 4",
        
        // Status
        statusOk: "OK",
        statusError: "ERRO",
        statusWarning: "AVISO",
        
        // Debug info
        rawValue: "Valor Bruto:",
        offset: "Offset:",
        instantPressure: "PressÃ£o InstantÃ¢nea:",
        samplesInAvg: "Amostras na MÃ©dia:",
        
        // Info messages
        debugInfo: "InformaÃ§Ãµes de debug atualizadas a 2Hz",
        lastUpdate: "Ãšltima atualizaÃ§Ã£o:",
        digitalInputsInfo: "Estado das Entradas Digitais - Atualiza a 2Hz",
        signalsUpdateInfo: "Sinais digitais atualizam automaticamente a 2Hz",
        signalsPage: "Sinais Digitais",
        signalsTitle: "Sinais Digitais - AirLab",
        
        // Digital inputs
        input1: "Entrada 1",
        input2: "Entrada 2",
        input3: "Entrada 3",
        input4: "Entrada 4",
        input5: "Entrada 5",
        input6: "Entrada 6",
        input7: "Entrada 7",
        input8: "Entrada 8",
        statusOn: "LIGADO",
        statusOff: "DESLIGADO",
        
        // Calibration instructions
        calibrationTitle: "InstruÃ§Ãµes de CalibraÃ§Ã£o",
        calibrationText: "Para calibrar: Exponha todas as ventosas Ã  pressÃ£o atmosfÃ©rica (sem vÃ¡cuo), depois clique em \"Calibrar Todos os Canais\" na pÃ¡gina principal para zerar as leituras.",
        movingAverage: "Filtro de mÃ©dia mÃ³vel: 10 amostras para leituras estÃ¡veis",
        systemStatus: "Estado do sistema: Certifique-se que",
        isRunning: "estÃ¡ em execuÃ§Ã£o!",
        
        // Confirmation dialogs
        shutdownConfirm: "Tem certeza que deseja desligar o Raspberry Pi?\n\nIsto irÃ¡ desligar o sistema completamente.",
        rebootConfirm: "Tem certeza que deseja reiniciar o Raspberry Pi?\n\nO sistema irÃ¡ reiniciar e a conexÃ£o serÃ¡ temporariamente perdida.",
        resetConfirm: "Resetar todos os offsets de calibraÃ§Ã£o para zero?",
        
        // Success/Error messages
        calibrationSuccess: "Todos os canais calibrados para 0 mbar",
        calibrationFailed: "Falha na calibraÃ§Ã£o:",
        resetSuccess: "CalibraÃ§Ã£o resetada para valores de fÃ¡brica",
        resetFailed: "Falha ao resetar:",
        shutdownSuccess: "Raspberry Pi estÃ¡ desligando...",
        shutdownFailed: "Falha ao desligar:",
        rebootSuccess: "Raspberry Pi estÃ¡ reiniciando...",
        rebootFailed: "Falha ao reiniciar:",
        connectionError: "Erro de conexÃ£o:",
        readError: "Falha ao ler dados do mestre EtherCAT"
    }
};

// Current language (default: English)
let currentLang = localStorage.getItem('airlab_language') || 'en';

// Get translation
function t(key) {
    return translations[currentLang][key] || key;
}

// Set language
function setLanguage(lang) {
    currentLang = lang;
    localStorage.setItem('airlab_language', lang);
    
    // Update active button
    document.querySelectorAll('.lang-btn').forEach(btn => {
        btn.classList.remove('active');
        if (btn.dataset.lang === lang) {
            btn.classList.add('active');
        }
    });
    
    // Trigger translation update
    updateTranslations();
}

// Update all text elements with translations
function updateTranslations() {
    // This function should be implemented in each HTML page
    // to update the specific elements
    if (typeof updatePageTranslations === 'function') {
        updatePageTranslations();
    }
}

// Initialize language on page load
document.addEventListener('DOMContentLoaded', function() {
    // Set active language button
    document.querySelectorAll('.lang-btn').forEach(btn => {
        if (btn.dataset.lang === currentLang) {
            btn.classList.add('active');
        }
    });
    
    // Update translations
    updateTranslations();
});
