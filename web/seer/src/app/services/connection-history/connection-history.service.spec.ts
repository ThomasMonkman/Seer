import { TestBed, inject } from '@angular/core/testing';

import { ConnectionHistoryService } from './connection-history.service';

describe('ConnectionHistoryService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [ConnectionHistoryService]
    });
  });

  it('should be created', inject([ConnectionHistoryService], (service: ConnectionHistoryService) => {
    expect(service).toBeTruthy();
  }));
});
